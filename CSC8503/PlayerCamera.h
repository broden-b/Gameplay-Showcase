#pragma once
#include "Camera.h"
#include "Window.h"
#include "PlayerController.h"

namespace NCL {
    namespace CSC8503 {
        class PlayerCamera {
        public:
            PlayerCamera() {}

            void Init(PlayerController* controller, Camera* gameCamera) {
                this->controller = controller;
                this->gameCamera = gameCamera;
                controller->SetCamera(this);
            }

            void UpdateCamera(float dt) {
                if (!controller || !gameCamera) return;

                GameObject* player = controller->GetPlayer();
                if (!player) return;

                float wheelMovement = Window::GetMouse()->GetWheelMovement();
                currentDistance = currentDistance - (wheelMovement * zoomSpeed);

                if (currentDistance < minDistance) currentDistance = minDistance;
                if (currentDistance > maxDistance) currentDistance = maxDistance;

                float mouseMoveX = Window::GetMouse()->GetRelativePosition().x;
                currentYaw += mouseMoveX * yawSensitivity * dt;

                Vector3 playerPos = player->GetTransform().GetPosition();
                Vector3 targetPos = playerPos + Vector3(0, heightOffset * 0.5f, 0);

                float camX = playerPos.x + (cos(currentYaw) * currentDistance);
                float camZ = playerPos.z + (sin(currentYaw) * currentDistance);
                Vector3 idealPos = Vector3(camX, playerPos.y + heightOffset, camZ);

                Vector3 currentPos = gameCamera->GetPosition();
                Vector3 newPos = currentPos + ((idealPos - currentPos) * (dt * cameraSmoothing));

                float yaw = (-currentYaw * RAD_TO_DEG) + 90;

                Vector3 lookDir = targetPos - newPos;
                float lookDirLength = sqrt(lookDir.x * lookDir.x + lookDir.y * lookDir.y + lookDir.z * lookDir.z);

                float pitch = 0.0f;
                if (lookDirLength > 0.0f) {
                    pitch = (-asin(lookDir.y / lookDirLength) * RAD_TO_DEG) - 30.0f;
                }

                gameCamera->SetPosition(newPos);
                gameCamera->SetPitch(pitch);
                gameCamera->SetYaw(yaw);
            }

            float GetYaw() const { return currentYaw; }

            void SetCameraDistance(float dist) {
                currentDistance = dist;
                if (currentDistance < minDistance) currentDistance = minDistance;
                if (currentDistance > maxDistance) currentDistance = maxDistance;
            }
            void SetHeightOffset(float offset) { heightOffset = offset; }
            void SetYawSensitivity(float sens) { yawSensitivity = sens; }
            void SetCameraSmoothing(float smooth) { cameraSmoothing = smooth; }
            void SetZoomSpeed(float speed) { zoomSpeed = speed; }
            void SetZoomLimits(float min, float max) {
                minDistance = min;
                maxDistance = max;
                if (currentDistance < minDistance) currentDistance = minDistance;
                if (currentDistance > maxDistance) currentDistance = maxDistance;
            }

        private:
            PlayerController* controller = nullptr;
            Camera* gameCamera = nullptr;

            float currentYaw = 0.0f;
            float heightOffset = 5.0f;
            float yawSensitivity = 2.0f;
            float cameraSmoothing = 5.0f;

            float currentDistance = 15.0f; 
            float minDistance = 5.0f;   
            float maxDistance = 30.0f;   
            float zoomSpeed = 1.0f;    

            const float RAD_TO_DEG = 57.2958f;
        };
    }
}