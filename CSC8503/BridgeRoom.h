#pragma once
#include "BaseRoom.h"
#include "PositionConstraint.h"

namespace NCL {
    namespace CSC8503 {
        class BridgeRoom : public BaseRoom {
        public:
            BridgeRoom(GameWorld* world, Vector3 position, Vector3 dimensions,
                Mesh* cubeMesh, Texture* basicTex, Shader* basicShader)
                : BaseRoom(world, position, dimensions, cubeMesh, basicTex, basicShader) {

                RemoveWall(NORTH);
                CreateRaisedFloor();

                CreateBridge();

                CreateStepBlock();
            }

        protected:
            void CreateStepBlock() {
                Vector3 stepPosition = position + Vector3(0, 5.0f, dimensions.z / 2 - 2.0f);
                Vector3 stepSize = Vector3(dimensions.x / 2, 2.5f, 2.0f);

                GameObject* step = new GameObject();

                AABBVolume* volume = new AABBVolume(stepSize);
                step->SetBoundingVolume((CollisionVolume*)volume);

                step->GetTransform()
                    .SetPosition(stepPosition)
                    .SetScale(stepSize * 2.0f);

                step->SetRenderObject(new RenderObject(&step->GetTransform(), cubeMesh, basicTex, basicShader));
                step->SetPhysicsObject(new PhysicsObject(&step->GetTransform(), step->GetBoundingVolume()));

                step->GetPhysicsObject()->SetInverseMass(0.0f);
                step->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(step);
            }

            void CreateRaisedFloor() {
                float floorHeight = 10.0f;
                float holeSize = 20.0f; 

                Vector3 floorThickness(dimensions.x / 2, 1.0f, dimensions.z / 2);

                CreateFloorSection(
                    position + Vector3(0, floorHeight, dimensions.z / 4),
                    Vector3(dimensions.x / 2, 1.0f, dimensions.z / 4 - holeSize / 2)
                );

                CreateFloorSection(
                    position + Vector3(0, floorHeight, -dimensions.z / 4),
                    Vector3(dimensions.x / 2, 1.0f, dimensions.z / 4 - holeSize / 2)
                );
            }

            void CreateFloorSection(const Vector3& position, const Vector3& dimensions) {
                GameObject* floor = new GameObject();

                AABBVolume* volume = new AABBVolume(dimensions);
                floor->SetBoundingVolume((CollisionVolume*)volume);

                floor->GetTransform()
                    .SetPosition(position)
                    .SetScale(dimensions * 2.0f);

                floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
                floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

                floor->GetPhysicsObject()->SetInverseMass(0.0f);
                floor->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(floor);
            }

            void CreateBridge() {
                Vector3 cubeSize = Vector3(2, 0.5f, 2);
                float invCubeMass = 5.0f;
                int numLinks = 4;
                float maxDistance = 6.0f;
                float cubeDistance = 5.0f;

                Vector3 startPos = position + Vector3(-15, 10, 0);

                GameObject* start = CreateBridgeAnchor(startPos);
                GameObject* end = CreateBridgeAnchor(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0));

                GameObject* previous = start;

                for (int i = 0; i < numLinks; ++i) {
                    GameObject* block = CreateBridgeBlock(
                        startPos + Vector3((i + 1) * cubeDistance, 0, 0),
                        cubeSize,
                        invCubeMass
                    );

                    PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
                    gameWorld->AddConstraint(constraint);
                    previous = block;
                }

                PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
                gameWorld->AddConstraint(constraint);
            }

            GameObject* CreateBridgeAnchor(const Vector3& position) {
                Vector3 cubeSize = Vector3(2, 0.5f, 2);
                GameObject* anchor = new GameObject();

                AABBVolume* volume = new AABBVolume(cubeSize);
                anchor->SetBoundingVolume((CollisionVolume*)volume);

                anchor->GetTransform()
                    .SetPosition(position)
                    .SetScale(cubeSize * 2.0f);

                anchor->SetRenderObject(new RenderObject(&anchor->GetTransform(), cubeMesh, basicTex, basicShader));
                anchor->SetPhysicsObject(new PhysicsObject(&anchor->GetTransform(), anchor->GetBoundingVolume()));

                anchor->GetPhysicsObject()->SetInverseMass(0.0f);
                anchor->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(anchor);
                return anchor;
            }

            GameObject* CreateBridgeBlock(const Vector3& position, const Vector3& size, float inverseMass) {
                GameObject* block = new GameObject();

                AABBVolume* volume = new AABBVolume(size);
                block->SetBoundingVolume((CollisionVolume*)volume);

                block->GetTransform()
                    .SetPosition(position)
                    .SetScale(size * 2.0f);

                block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
                block->SetPhysicsObject(new PhysicsObject(&block->GetTransform(), block->GetBoundingVolume()));

                block->GetPhysicsObject()->SetInverseMass(inverseMass);
                block->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(block);
                return block;
            }
        };
    }
}