#pragma once
#include "PushdownState.h"
#include "PushdownMachine.h"
#include "Window.h"

namespace NCL {
    namespace CSC8503 {
        class MenuState;
        class GameState;

        class MenuSystem {
        public:
            MenuSystem();
            ~MenuSystem();

            void UpdateMenu(float dt);
            bool IsGameRunning() const { return isRunning; }
            PushdownState* GetCurrentState() const {
                return menuMachine->GetCurrentState();
            }

            void SetNewState(PushdownState* state) {
                if (menuMachine) {
                    delete menuMachine;
                }
                menuMachine = new PushdownMachine(state);
            }

        protected:
            PushdownMachine* menuMachine;
            bool isRunning;
        };

        class MenuState : public PushdownState {
        public:
            MenuState() {}
            ~MenuState() {}

            virtual PushdownState::PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override;
            void OnAwake() override;
        };

        class GameState : public PushdownState {
        public:
            GameState() {}
            ~GameState() {}

            virtual PushdownState::PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override;
            void OnAwake() override;
        };

        class GameOverState : public PushdownState {
        public:
            GameOverState(int finalScore) : finalScore(finalScore) {}
            ~GameOverState() {}

            virtual PushdownState::PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override;
            void OnAwake() override;

        private:
            int finalScore;
        };
    }
}