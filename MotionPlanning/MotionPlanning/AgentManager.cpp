#include "AgentManager.h"
#include "Utils.h"

bool AgentManager::AgentsRunning = false;

AgentManager::AgentManager(MotionPlanner* motionPlanner) : _motionPlanner(motionPlanner) {
    InitializeAgents();
}

void AgentManager::Update() {
    for (const auto& agentGroup : _agentGroups) {
        for (const auto& agent : agentGroup) {
            agent->Update();
        }
    }
}

void AgentManager::Reset() {
    for (const auto& agentGroup : _agentGroups) {
        for (const auto& agent : agentGroup) {
            agent->Reset();
        }
    }
}

std::vector<Agent*> AgentManager::GetAllAgentsWithinDistanceOf(Agent* me, float distance) {
    std::vector<Agent*> inRange = std::vector<Agent*>();
    for (const auto& agentGroup : _agentGroups) {
        for (const auto& agent : agentGroup) {
            if (agent != me && me->DistanceFrom(agent) < distance) {
                inRange.push_back(agent);
            }
        }
    }

    return inRange;
}

void AgentManager::SetNewGroupGoal(Agent* agent) {
    /*if (_agentGroups.size() == 1) {
        agent->SetGoal(_motionPlanner->GetRandomValidPoint());
        return;
    }*/

    for (const auto& agentGroup : _agentGroups) {
        if (std::find(agentGroup.begin(), agentGroup.end(), agent) != agentGroup.end()) {
            auto newGoal = _motionPlanner->GetRandomValidPoint();
            for (const auto& agentInGroup : agentGroup) {
                auto perturb = Utils::RandomVector();
                while (_motionPlanner->_cSpace.PointIsInsideObstacle(newGoal + perturb)) {
                    perturb = Utils::RandomVector();
                }
                agentInGroup->SetGoal(newGoal + perturb);
            }
        }
    }
}

void AgentManager::InitializeAgents() {
    for (int i = 0; i < 1; i++) {
        _agentGroups.emplace_back();
        glm::vec3 groupGoal = _motionPlanner->GetRandomValidPoint();
        // glm::vec3 groupStart = _motionPlanner->GetRandomValidPoint();
        glm::vec3 groupStart = glm::vec3(0, 0, 0);
        auto color = Utils::RandomVector01();

        for (int j = 0; j < 5; j++) {
            auto newAgent = new Agent(groupStart, groupGoal, _motionPlanner, this);
            // newAgent->SetColor(color);
            _agentGroups[i].push_back(newAgent);
        }
    }
}
