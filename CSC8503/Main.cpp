#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"

#include "PushdownMachine.h"
#include "PushdownState.h"
#include "MenuSystem.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

int main() {
	WindowInitialisation initInfo;
	initInfo.width = 1920;
	initInfo.height = 1080;
	initInfo.fullScreen = false;
	initInfo.windowTitle = "CSC8503 Game technology!";

	Window* w = Window::CreateGameWindow(initInfo);

	if (!w->HasInitialised()) {
		return -1;
	}

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	MenuSystem* menuSystem = new MenuSystem();
	TutorialGame* g = new TutorialGame(menuSystem);

	w->GetTimer().GetTimeDeltaSeconds();

	while (w->UpdateWindow() && menuSystem->IsGameRunning()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			continue;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
			w->ShowConsole(false);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		menuSystem->UpdateMenu(dt);
		
		if (dynamic_cast<GameState*>(menuSystem->GetCurrentState())) {
			g->UpdateGame(dt);
		}
		else {
			g->RenderGame();
		}
	}
	Window::DestroyGameWindow();
	return 0;
}