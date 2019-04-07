#include "AgentManager.h"
#include "Utils.h"

bool AgentManager::AgentsRunning = false;

AgentManager::AgentManager(MotionPlanner* motionPlanner) : _motionPlanner(motionPlanner) {
    InitializeAgents();
}

void AgentManager::Update() {
    for (Agent* agent : _agents) {
        agent->Update();
    }
}

void AgentManager::Reset() {
    for (Agent* agent : _agents) {
        agent->Reset();
    }
}

std::vector<Agent*> AgentManager::GetAllAgentsWithinDistanceOf(Agent* me, float distance) {
    std::vector<Agent*> inRange = std::vector<Agent*>();
    for (Agent* agent : _agents) {
        if (agent != me && me->DistanceFrom(agent) < distance) {
            inRange.push_back(agent);
        }
    }

    return inRange;
}

void AgentManager::InitializeAgents() {
    for (int i = 0; i < 10; i++) {
        _agents.push_back(new Agent(glm::vec3(i - 10, -10, -10 + Utils::Random() * 10), glm::vec3(10, 10, 10), _motionPlanner, this));
        _agents.push_back(new Agent(glm::vec3(i, -10, 10 - Utils::Random() * 10), glm::vec3(-10, 10, -10), _motionPlanner, this));
    }
}
