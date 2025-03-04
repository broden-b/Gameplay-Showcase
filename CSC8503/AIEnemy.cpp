#include "AIEnemy.h"
#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

AIEnemy::AIEnemy(GameWorld* world) : GameObject("AIEnemy") {
    gameWorld = world;
    currentPatrolPoint = 0;
    patrolCounter = 0;
    sleepTimer = 0.0f;
    pursuitTimer = 0.0f;
    currentState = PATROL;
    playerObject = nullptr;
    InitBehaviourTree();
}

AIEnemy::~AIEnemy() {
    delete rootSequence;
}

void AIEnemy::Update(float dt) {

    std::string stateStr;
    switch (currentState) {
    case PATROL:  stateStr = "PATROL"; break;
    case SLEEP:   stateStr = "SLEEP"; break;
    case PURSUIT: stateStr = "PURSUIT"; break;
    }

    rootSequence->Execute(dt);
}

void AIEnemy::SetPatrolPoints(const std::vector<Vector3>& points) {
    patrolPoints = points;
}

void AIEnemy::SetPlayerObject(GameObject* player) {
    playerObject = player;
}

bool AIEnemy::CanSeePlayer() const {
    if (!playerObject) return false;

    Vector3 direction = playerObject->GetTransform().GetPosition() - transform.GetPosition();

    if (Vector::Length(direction) > VISION_RANGE) return false;

    Ray ray(transform.GetPosition(), Vector::Normalise(direction));
    RayCollision closestCollision;

    if (gameWorld->Raycast(ray, closestCollision, true, (GameObject*)this)) {
        if (closestCollision.node == playerObject) {
            return true;
        }
    }

    return false;
}

void AIEnemy::InitBehaviourTree() {
    BehaviourAction* patrolAction = new BehaviourAction("Patrol",
        [&](float dt, BehaviourState state) -> BehaviourState {
            if (currentState != PATROL) {
                return Failure;
            }

            if (CanSeePlayer()) {
                currentState = PURSUIT;
                pursuitTimer = MAX_PURSUIT_TIME;
                return Success;
            }

            Vector3 targetPoint = patrolPoints[currentPatrolPoint];
            Vector3 direction = targetPoint - transform.GetPosition();
            float distance = Vector::Length(direction);

            if (distance < 1.0f) {
                currentPatrolPoint = (currentPatrolPoint + 1) % patrolPoints.size();
                patrolCounter++;

                if (patrolCounter >= patrolPoints.size()) {
                    currentState = SLEEP;
                    sleepTimer = MAX_SLEEP_TIME;
                    patrolCounter = 0;
                    return Success;
                }
            }

            Vector3 force = Vector::Normalise(direction) * 100.0f;
            GetPhysicsObject()->AddForce(force);

            Vector3 currentVel = GetPhysicsObject()->GetLinearVelocity();
            GetPhysicsObject()->AddForce(-currentVel * 10.0f);

            return Ongoing;
        });

    BehaviourAction* sleepAction = new BehaviourAction("Sleep",
        [&](float dt, BehaviourState state) -> BehaviourState {
            if (currentState != SLEEP) {
                return Failure;
            }

            sleepTimer -= dt;
            if (sleepTimer <= 0.0f) {
                currentState = PATROL;
                return Success;
            }
            return Ongoing;
        });

    BehaviourAction* pursuitAction = new BehaviourAction("Pursuit",
        [&](float dt, BehaviourState state) -> BehaviourState {
            if (currentState != PURSUIT) {
                return Failure;
            }

            pursuitTimer -= dt;
            if (pursuitTimer <= 0.0f) {
                currentState = PATROL;
                return Success;
            }

            if (playerObject) {
                Vector3 direction = playerObject->GetTransform().GetPosition() - transform.GetPosition();
                Vector3 force = Vector::Normalise(direction) * 100.0f;
                GetPhysicsObject()->AddForce(force);

                Vector3 currentVel = GetPhysicsObject()->GetLinearVelocity();
                GetPhysicsObject()->AddForce(-currentVel * 10.0f);
            }
            return Ongoing;
        });

    modeSelector = new BehaviourSelector("Mode Selector");
    modeSelector->AddChild(pursuitAction);
    modeSelector->AddChild(sleepAction);
    modeSelector->AddChild(patrolAction);

    rootSequence = new BehaviourSequence("Root");
    rootSequence->AddChild(modeSelector);
}