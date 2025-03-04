#include "PlayerController.h"
#include "PlayerCamera.h"
#include "Window.h"
#include "PhysicsObject.h"
#include "CollisionDetection.h"
#include <cmath>

using namespace NCL;
using namespace CSC8503;

PlayerController::PlayerController(GameObject* player, GameWorld* world) :
    player(player), gameWorld(world), camera(nullptr) {
}

void PlayerController::Update(float dt) {
    if (!player || !gameWorld) return;

    Move(dt);

    if (jumpTimer > 0) {
        jumpTimer -= dt;
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE) && jumpTimer <= 0) {
        Jump();
    }
}

void PlayerController::Move(float dt) {
    if (!player || !camera) return;

    PhysicsObject* phys = player->GetPhysicsObject();
    if (!phys) return;

    Vector3 movement;
    bool moving = false;

    if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
        movement.x -= 1.0f;
        moving = true;
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
        movement.x += 1.0f;
        moving = true;
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
        movement.z += 1.0f;
        moving = true;
    }
    if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
        movement.z -= 1.0f;
        moving = true;
    }

    if (moving) {
        float magnitude = sqrt(movement.x * movement.x + movement.z * movement.z);
        if (magnitude > 0) {
            movement.x /= magnitude;
            movement.z /= magnitude;

            float cameraYaw = camera->GetYaw();
            float cosYaw = std::cos(cameraYaw);
            float sinYaw = std::sin(cameraYaw);

            Vector3 rotatedMovement;
            rotatedMovement.x = movement.x * cosYaw - movement.z * sinYaw;
            rotatedMovement.z = movement.x * sinYaw + movement.z * cosYaw;
            rotatedMovement.y = 0.0f;

            float targetYaw = (std::atan2(rotatedMovement.x, rotatedMovement.z));

            float halfYaw = targetYaw * 0.5f;
            float sinHalfYaw = std::sin(halfYaw);
            float cosHalfYaw = std::cos(halfYaw);

            Quaternion targetRotation = Quaternion(0, sinHalfYaw, 0, cosHalfYaw);
            targetRotation.Normalise();

            Vector3 force = rotatedMovement * movementSpeed;
            phys->AddForce(force);
        }
    }
}

void PlayerController::Jump() {
    if (!player) return;
    PhysicsObject* phys = player->GetPhysicsObject();
    if (!phys) return;

    phys->AddForce(Vector3(0, jumpForce, 0));
    jumpTimer = jumpCooldown;
}