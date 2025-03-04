#pragma once

namespace NCL {
	namespace CSC8503 {
		class PushdownState;

        class PushdownMachine {
        public:
            PushdownMachine(PushdownState* initialState);
            ~PushdownMachine();

            bool Update(float dt);
            PushdownState* GetCurrentState() const { return activeState; }

        protected:
            PushdownState* activeState;
            PushdownState* initialState;

            std::stack<PushdownState*> stateStack;
        };
	}
}

