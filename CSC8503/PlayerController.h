#pragma once
#include "GameObject.h"
#include "GameWorld.h"

namespace NCL {
    namespace CSC8503 {
        class PlayerCamera;

        class PlayerController {
        public:
            PlayerController(GameObject* player, GameWorld* world);

            void Update(float dt);
            void Move(float dt);
            void Jump();

            void SetCamera(PlayerCamera* cam) { camera = cam; }
            GameObject* GetPlayer() const { return player; }

            float GetMovementSpeed() const { return movementSpeed; }
            void SetMovementSpeed(float speed) { movementSpeed = speed; }

            float GetJumpForce() const { return jumpForce; }
            void SetJumpForce(float force) { jumpForce = force; }

            void SetRotationSpeed(float speed) { rotationSpeed = speed; }

        private:
            GameObject* player;
            GameWorld* gameWorld;
            PlayerCamera* camera;

            float movementSpeed = 15.0f;
            float jumpForce = 1500.0f;
            float rotationSpeed = 10.0f;

            float jumpTimer = 0.0f;
            const float jumpCooldown = 0.5f;
        };
    }
}