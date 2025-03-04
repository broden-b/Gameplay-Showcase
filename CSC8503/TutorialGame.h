#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#include "PhysicsSystem.h"

#include "PlayerController.h"
#include "PendulumRoom.h"
#include "BridgeRoom.h"
#include "AIRoom.h"
#include "MenuSystem.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerCamera;
		class PlayerController;
		class TutorialGame		{
		public:
			TutorialGame(MenuSystem* menu);
			~TutorialGame();

			virtual void UpdateGame(float dt);
			void RenderGame() {
				renderer->Update(0.0f);
				renderer->Render();
				Debug::UpdateRenderables(0.0f);
			}

		protected:
			void InitialiseAssets();

			void InitCamera();

			void InitWorld();

			void InitDefaultFloor();

			void CreateCollisionRoom();
			void CreatePendulumRoom();
			void CreateBridgeRoom();
			void CreateAIRoom();

			void UpdateScore(int points);
			void CheckKittenCollisions();

			GameObject* AddFloorToWorld(const Vector3& position);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddKittenToWorld(const Vector3& position);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			KeyboardMouseController controller;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;

			Texture*	basicTex	= nullptr;
			Shader*		basicShader = nullptr;

			Mesh*	catMesh		= nullptr;
			Mesh*	kittenMesh	= nullptr;
			Mesh*	enemyMesh	= nullptr;

			GameObject* objClosest = nullptr;
			GameObject* playerObject;

			MenuSystem* menuSystem = nullptr;

			PendulumRoom* pendulumRoom = nullptr;
			BridgeRoom* bridgeRoom = nullptr;
			AIRoom* aiRoom;

			PlayerController* playerController;
			PlayerCamera* playerCamera;

			int score = 0;
			std::vector<GameObject*> kittens;

			float gameTimer = 180.0f;
			bool isGameOver = false;
		};
	}
}

