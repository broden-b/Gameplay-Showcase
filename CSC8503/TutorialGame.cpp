#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "TutorialGame.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "CollisionRoom.h"



using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(MenuSystem* menu) : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	menuSystem = menu;
	world		= new GameWorld();

	renderer = new GameTechRenderer(*world);

	physics		= new PhysicsSystem(*world);

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
}

void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	catMesh		= renderer->LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh	= renderer->LoadMesh("Kitten.msh");

	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete catMesh;
	delete kittenMesh;
	delete enemyMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	delete pendulumRoom;
	delete aiRoom;
	delete bridgeRoom;
}

void TutorialGame::UpdateGame(float dt) {
	if (isGameOver) return;
	if (playerCamera) {
		playerCamera->UpdateCamera(dt);
	}
	else {
		world->GetMainCamera().UpdateCamera(dt);
	}


	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::Print("Score: " + std::to_string(score), Vector2(80, 5));

	gameTimer -= dt;
	int minutes = static_cast<int>(gameTimer) / 60;
	int seconds = static_cast<int>(gameTimer) % 60;
	std::string timeStr = std::to_string(minutes) + ":" +
		(seconds < 10 ? "0" : "") +
		std::to_string(seconds);
	Debug::Print("Time: " + timeStr, Vector2(80, 10), Vector4(1, 1, 1, 1));

	if (gameTimer <= 0 || score >= 4) {
		isGameOver = true;
		if (menuSystem) {
			menuSystem->GetCurrentState()->OnSleep();
			delete menuSystem->GetCurrentState();
			menuSystem->SetNewState(new GameOverState(score));
		}
	}

	world->UpdateWorld(dt);
	physics->Update(dt);
	CheckKittenCollisions();

	if (pendulumRoom) {
		pendulumRoom->UpdateSphere(dt);
	}
	if (aiRoom) {
		aiRoom->UpdateEnemy(dt);
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	
	if (playerObject) {
		aiRoom->SetPlayer(playerObject);
	}

	RenderGame();

	if (playerController) {
		playerController->Update(dt);
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-15.0f);
	world->GetMainCamera().SetYaw(315.0f);
	world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	InitDefaultFloor();
	AddPlayerToWorld(Vector3(0, 5, 0));
	CreateCollisionRoom();
	CreatePendulumRoom();
	CreateBridgeRoom();
	CreateAIRoom();
	AddKittenToWorld(Vector3(2, 3, 29));
	AddKittenToWorld(Vector3(-20, 3, -33));
	AddKittenToWorld(Vector3(-75, 12, -52));
	AddKittenToWorld(Vector3(80, 3, 28));

}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(500, 2, 500);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject("Player");
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);
	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), catMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);
	playerObject = character;

	playerController = new PlayerController(character, world);
	playerCamera = new PlayerCamera();
	playerCamera->Init(playerController, &world->GetMainCamera());

	Vector3 initialCamPos = position + Vector3(-15, 5, -15);
	world->GetMainCamera().SetPosition(initialCamPos);
	world->GetMainCamera().SetPitch(20.0f);
	world->GetMainCamera().SetYaw(45.0f);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddKittenToWorld(const Vector3& position) {
	GameObject* kitten = new GameObject("Kitten");

	SphereVolume* volume = new SphereVolume(0.5f);
	kitten->SetBoundingVolume((CollisionVolume*)volume);

	kitten->GetTransform()
		.SetScale(Vector3(0.5f, 0.5f, 0.5f))
		.SetPosition(position);

	kitten->SetRenderObject(new RenderObject(&kitten->GetTransform(), kittenMesh, nullptr, basicShader));
	kitten->SetPhysicsObject(new PhysicsObject(&kitten->GetTransform(), kitten->GetBoundingVolume()));

	kitten->GetPhysicsObject()->SetInverseMass(0.0f);
	kitten->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(kitten);
	kittens.push_back(kitten);
	return kitten;
}

void TutorialGame::CheckKittenCollisions() {
	if (!playerObject) return;

	auto it = kittens.begin();
	while (it != kittens.end()) {
		GameObject* kitten = *it;

		CollisionDetection::CollisionInfo info;
		if (CollisionDetection::ObjectIntersection(playerObject, kitten, info)) {
			world->RemoveGameObject(kitten, true);
			it = kittens.erase(it);
			UpdateScore(1);
		}
		else {
			++it;
		}
	}
}

void TutorialGame::UpdateScore(int points) {
	score += points;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, 0, 0));
}

void TutorialGame::CreateCollisionRoom() {
	Vector3 roomPos = Vector3(20, 0, 30);
	Vector3 roomDims = Vector3(50, 20, 20);
	CollisionRoom* room = new CollisionRoom(
		world,              
		roomPos,           
		roomDims,          
		cubeMesh,          
		basicTex,           
		basicShader 
	);
}

void TutorialGame::CreatePendulumRoom() {
	Vector3 roomPos = Vector3(-20, 0, -40);
	Vector3 roomDims = Vector3(7, 30, 40);

	pendulumRoom = new PendulumRoom(
		world,
		roomPos,
		roomDims,
		cubeMesh,
		sphereMesh,
		basicTex,
		basicShader
	);
}

void TutorialGame::CreateBridgeRoom() {
	Vector3 roomPos = Vector3(-75, 0, -40);
	Vector3 roomDims = Vector3(30, 20, 50);

	bridgeRoom = new BridgeRoom(
		world,
		roomPos,
		roomDims,
		cubeMesh,
		basicTex,
		basicShader
	);
}

void TutorialGame::CreateAIRoom() {
	Vector3 roomPos = Vector3(80, 0, 30);
	Vector3 roomDims = Vector3(30, 20, 30);

	aiRoom = new AIRoom(
		world,
		roomPos,
		roomDims,
		cubeMesh,
		enemyMesh,
		basicTex,
		basicShader
	);
}