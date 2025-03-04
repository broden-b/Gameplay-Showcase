#pragma once
#include "TutorialGame.h"
#include "BaseRoom.h"

namespace NCL {
    namespace CSC8503 {
        class CollisionRoom : public BaseRoom {
        public:
            CollisionRoom(GameWorld* world, Vector3 position, Vector3 dimensions,
                Mesh* cubeMesh, Texture* basicTex, Shader* basicShader)
                : BaseRoom(world, position, dimensions, cubeMesh, basicTex, basicShader) {
               
                RemoveWall(EAST);                
                CreateDividingWall();
              
                CreatePushableCube();
                CreateSecondCube();
            }
           
            GameObject* GetPushableCube() const {
                return pushableCube;
            }

        protected:
            void CreateDividingWall() {              
                Vector3 wallDimensions = Vector3(1.0f, dimensions.y * 0.25f, dimensions.z * 0.5f);
                Vector3 wallPosition = position + Vector3(0, wallDimensions.y, 0);

                GameObject* divider = new GameObject();

                AABBVolume* volume = new AABBVolume(wallDimensions);
                divider->SetBoundingVolume((CollisionVolume*)volume);

                divider->GetTransform()
                    .SetPosition(wallPosition)
                    .SetScale(wallDimensions * 2.0f);

                divider->SetRenderObject(new RenderObject(&divider->GetTransform(), cubeMesh, basicTex, basicShader));
                divider->SetPhysicsObject(new PhysicsObject(&divider->GetTransform(), divider->GetBoundingVolume()));

                divider->GetPhysicsObject()->SetInverseMass(0.0f);
                divider->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(divider);
                dividingWall = divider;
            }

            void CreatePushableCube() {
                
                Vector3 cubeSize = Vector3(2.0f, 2.0f, 2.0f);
                Vector3 cubePosition = position +
                    Vector3(dimensions.x * 0.3f, cubeSize.y, 0);

                GameObject* cube = new GameObject();

                AABBVolume* volume = new AABBVolume(cubeSize);
                cube->SetBoundingVolume((CollisionVolume*)volume);

                cube->GetTransform()
                    .SetPosition(cubePosition)
                    .SetScale(cubeSize * 2.0f);

                cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
                cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
                
                cube->GetPhysicsObject()->SetInverseMass(0.1f);
                cube->GetPhysicsObject()->InitCubeInertia();

                gameWorld->AddGameObject(cube);
                pushableCube = cube;
            }

            void CreateSecondCube() {
                Vector3 cubeSize = Vector3(2.0f, 2.0f, 2.0f);
                Vector3 cubePosition = position +
                    Vector3(-dimensions.x * 0.25f, cubeSize.y, 0);

                GameObject* cube = new GameObject();

                AABBVolume* volume = new AABBVolume(cubeSize);
                cube->SetBoundingVolume((CollisionVolume*)volume);

                cube->GetTransform()
                    .SetPosition(cubePosition)
                    .SetScale(cubeSize * 2.0f);

                cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
                cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
         
                cube->GetPhysicsObject()->SetInverseMass(0.1f);
                cube->GetPhysicsObject()->InitCubeInertia();           

                gameWorld->AddGameObject(cube);
            }

        private:
            GameObject* dividingWall = nullptr;
            GameObject* pushableCube = nullptr;
        };
    }
}