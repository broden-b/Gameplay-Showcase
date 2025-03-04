#pragma once
#include "BaseRoom.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

namespace NCL {
    namespace CSC8503 {
        class MovingSphere : public GameObject {
        public:
            MovingSphere(const std::string& name = "") : GameObject(name) {
                stateMachine = new StateMachine();

                State* moveUpState = new State([&](float dt) {
                    GetPhysicsObject()->SetLinearVelocity(Vector3(0, 10.0f, 0));
                    });

                State* moveDownState = new State([&](float dt) {
                    GetPhysicsObject()->SetLinearVelocity(Vector3(0, -10.0f, 0));
                    });

                stateMachine->AddState(moveUpState);
                stateMachine->AddState(moveDownState);

                stateMachine->AddTransition(new StateTransition(moveUpState, moveDownState,
                    [&]() {
                        return GetTransform().GetPosition().y >= startPosition.y + 15.0f;
                    }
                ));

                stateMachine->AddTransition(new StateTransition(moveDownState, moveUpState,
                    [&]() {
                        return GetTransform().GetPosition().y <= startPosition.y - 5.5f;
                    }
                ));
            }

            ~MovingSphere() {
                delete stateMachine;
            }

            void Update(float dt) {
                stateMachine->Update(dt);
            }

            void SetStartPosition(const Vector3& pos) {
                startPosition = pos;
            }

        protected:
            StateMachine* stateMachine;
            Vector3 startPosition;
        };

        class PendulumRoom : public BaseRoom {
        public:
            PendulumRoom(GameWorld* world, Vector3 position, Vector3 dimensions,
                Mesh* cubeMesh, Mesh* sphereMesh, Texture* basicTex, Shader* basicShader)
                : BaseRoom(world, position, dimensions, cubeMesh, basicTex, basicShader) {

                RemoveWall(SOUTH);
                CreatePendulumSphere(sphereMesh);
            }

            ~PendulumRoom() {}

            void UpdateSphere(float dt) {
                if (pendulumSphere) {
                    pendulumSphere->Update(dt);
                }
            }

            MovingSphere* GetPendulumSphere() const {
                return pendulumSphere;
            }

        protected:
            void CreatePendulumSphere(Mesh* sphereMesh) {
                Vector3 spherePos = position + Vector3(0, 10.0f, dimensions.z * -0.25f);

                pendulumSphere = new MovingSphere("PendulumSphere");
                pendulumSphere->SetStartPosition(spherePos);

                SphereVolume* volume = new SphereVolume(2.0f);
                pendulumSphere->SetBoundingVolume((CollisionVolume*)volume);

                pendulumSphere->GetTransform()
                    .SetScale(Vector3(2, 2, 2))
                    .SetPosition(spherePos);

                pendulumSphere->SetRenderObject(new RenderObject(&pendulumSphere->GetTransform(), sphereMesh, basicTex, basicShader));
                pendulumSphere->SetPhysicsObject(new PhysicsObject(&pendulumSphere->GetTransform(), pendulumSphere->GetBoundingVolume()));

                pendulumSphere->GetPhysicsObject()->SetInverseMass(1.0f);
                pendulumSphere->GetPhysicsObject()->InitSphereInertia();

                gameWorld->AddGameObject(pendulumSphere);
            }

        private:
            MovingSphere* pendulumSphere = nullptr;
        };
    }
}