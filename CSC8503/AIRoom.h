#pragma once
#include "BaseRoom.h"
#include "AIEnemy.h"

namespace NCL {
    namespace CSC8503 {
        class AIRoom : public BaseRoom {
        public:
            AIRoom(GameWorld* world, Vector3 position, Vector3 dimensions,
                Mesh* cubeMesh, Mesh* enemyMesh, Texture* basicTex, Shader* basicShader)
                : BaseRoom(world, position, dimensions, cubeMesh, basicTex, basicShader) {

                RemoveWall(SOUTH);
                CreateAIEnemy(enemyMesh);
                SetupPatrolPoints();
            }

            ~AIRoom() {}

            void UpdateEnemy(float dt) {
                if (enemy) {
                    enemy->Update(dt);
                }
            }

            void SetPlayer(GameObject* player) {
                if (enemy) {
                    enemy->SetPlayerObject(player);
                }
            }

        protected:
            void CreateAIEnemy(Mesh* enemyMesh) {
                Vector3 enemyPos = position + Vector3(0, 5.0f, 0);

                enemy = new AIEnemy(gameWorld);

                SphereVolume* volume = new SphereVolume(1.0f);
                enemy->SetBoundingVolume((CollisionVolume*)volume);

                enemy->GetTransform()
                    .SetScale(Vector3(2, 2, 2))
                    .SetPosition(enemyPos);

                enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(),
                    enemyMesh, basicTex, basicShader));

                enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(),
                    enemy->GetBoundingVolume()));

                enemy->GetPhysicsObject()->SetInverseMass(0.5f);
                enemy->GetPhysicsObject()->InitSphereInertia();

                gameWorld->AddGameObject(enemy);
            }

            void SetupPatrolPoints() {
                float patrolOffset = dimensions.x * 0.3f;
                std::vector<Vector3> points = {
                    position + Vector3(patrolOffset, 3.0f, patrolOffset),
                    position + Vector3(patrolOffset, 3.0f, -patrolOffset),
                    position + Vector3(-patrolOffset, 3.0f, -patrolOffset),
                    position + Vector3(-patrolOffset, 3.0f, patrolOffset)
                };

                enemy->SetPatrolPoints(points);
            }

        private:
            AIEnemy* enemy = nullptr;
        };
    }
}