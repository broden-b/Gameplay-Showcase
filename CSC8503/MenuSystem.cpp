#include "MenuSystem.h"
#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

MenuSystem::MenuSystem() {
    menuMachine = new PushdownMachine(new MenuState());
    isRunning = true;
}

MenuSystem::~MenuSystem() {
    delete menuMachine;
}

void MenuSystem::UpdateMenu(float dt) {
    if (!menuMachine->Update(dt)) {
        isRunning = false;
    }
}

PushdownState::PushdownResult MenuState::OnUpdate(float dt, PushdownState** pushFunc) {
    Debug::Print("MAIN MENU", Vector2(45, 30), Vector4(1, 1, 0, 1));
    Debug::Print("Press SPACE to Start Game", Vector2(35, 40), Vector4(1, 1, 1, 1));
    Debug::Print("Press ESCAPE to Quit", Vector2(35, 50), Vector4(1, 1, 1, 1));

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) {
        *pushFunc = new GameState();
        return PushdownResult::Push;
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
        Window::DestroyGameWindow();
        exit(0);
    }

    return PushdownResult::NoChange;
}

void MenuState::OnAwake() {
    Window::GetWindow()->ShowOSPointer(true);
    Window::GetWindow()->LockMouseToWindow(false);
}

// Game State Implementation
PushdownState::PushdownResult GameState::OnUpdate(float dt, PushdownState** pushFunc) {
    Debug::Print("Press ESCAPE to return to menu", Vector2(5, 5), Vector4(1, 1, 1, 1));

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
        *pushFunc = new MenuState();
        return PushdownResult::Pop;
    }
    return PushdownResult::NoChange;
}

void GameState::OnAwake() {
    Window::GetWindow()->ShowOSPointer(false);
    Window::GetWindow()->LockMouseToWindow(true);
}

PushdownState::PushdownResult GameOverState::OnUpdate(float dt, PushdownState** pushFunc) {
    Debug::Print("GAME OVER!", Vector2(45, 30), Vector4(1, 0, 0, 1)); // Red text
    Debug::Print("Final Score: " + std::to_string(finalScore), Vector2(40, 40), Vector4(1, 1, 1, 1));
    Debug::Print("Thank you for playing!", Vector2(35, 50), Vector4(1, 1, 0, 1));
    Debug::Print("Press ESCAPE to Exit", Vector2(35, 70), Vector4(1, 1, 1, 1));

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
        Window::DestroyGameWindow();
        exit(0);
    }

    return PushdownResult::NoChange;
}

void GameOverState::OnAwake() {
    Window::GetWindow()->ShowOSPointer(true);
    Window::GetWindow()->LockMouseToWindow(false);
}