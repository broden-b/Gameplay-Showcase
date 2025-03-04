#pragma once
#include "GameObject.h"
#include "GameWorld.h"
#include "AABBVolume.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

namespace NCL {
    namespace CSC8503 {
        class BaseRoom {
        public:
            enum WallType {
                NORTH = 0,
                SOUTH = 1,
                EAST = 2,
                WEST = 3,
                CEILING = 4
            };

            BaseRoom(GameWorld* world, Vector3 position, Vector3 dimensions,
                Mesh* cubeMesh, Texture* basicTex, Shader* basicShader) {
                this->gameWorld = world;
                this->position = position;
                this->dimensions = dimensions;
                this->cubeMesh = cubeMesh;
                this->basicTex = basicTex;
                this->basicShader = basicShader;

                CreateRoom();
            }

            virtual ~BaseRoom() {
            }

            void RemoveWall(WallType wall) {
                if (wall >= 0 && wall < 5 && walls[wall]) {
                    gameWorld->RemoveGameObject(walls[wall], true);
                    walls[wall] = nullptr;
                }
            }

            GameObject* GetWall(WallType wall) const {
                if (wall >= 0 && wall < 5) {
                    return walls[wall];
                }
                return nullptr;
            }

            Vector3 GetPosition() const {
                return position;
            }

            Vector3 GetDimensions() const {
                return dimensions;
            }

        protected:
            void CreateRoom() {
                float wallThickness = 1.0f;

                walls[NORTH] = CreateWall(
                    position + Vector3(0, dimensions.y / 2, dimensions.z / 2),
                    Vector3(dimensions.x / 2, dimensions.y / 2, wallThickness)
                );

                walls[SOUTH] = CreateWall(
                    position + Vector3(0, dimensions.y / 2, -dimensions.z / 2),
                    Vector3(dimensions.x / 2, dimensions.y / 2, wallThickness)
                );

                walls[EAST] = CreateWall(
                    position + Vector3(dimensions.x / 2, dimensions.y / 2, 0),
                    Vector3(wallThickness, dimensions.y / 2, dimensions.z / 2)
                );

                walls[WEST] = CreateWall(
                    position + Vector3(-dimensions.x / 2, dimensions.y / 2, 0),
                    Vector3(wallThickness, dimensions.y / 2, dimensions.z / 2)
                );

                walls[CEILING] = CreateWall(
                    position + Vector3(0, dimensions.y, 0),
                    Vector3(dimensions.x / 2, wallThickness, dimensions.z / 2)
                );
            }

            GameObject* CreateWall(const Vector3& position, const Vector3& dimensions) {
                GameObject* wall = new GameObject();

                AABBVolume* volume = new AABBVolume(dimensions);
                wall->SetBoundingVolume((CollisionVolume*)volume);

                wall->GetTransform()
                    .SetPosition(position)
                    .SetScale(dimensions * 2.0f);

                wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
                wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

                wall->GetPhysicsObject()->SetInverseMass(0.0f);
                wall->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(wall);
                return wall;
            }

            GameWorld* gameWorld;
            Vector3 position;
            Vector3 dimensions;
            GameObject* walls[5];

            Mesh* cubeMesh;
            Texture* basicTex;
            Shader* basicShader;
        };
    }
}