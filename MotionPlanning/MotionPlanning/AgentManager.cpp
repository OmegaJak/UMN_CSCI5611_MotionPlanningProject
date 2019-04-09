#include "AgentManager.h"
#include "Utils.h"

bool AgentManager::AgentsRunning = false;

AgentManager::AgentManager(MotionPlanner* motionPlanner, Environment* environment)
    : _motionPlanner(motionPlanner), _environment(environment) {
    InitializeAgents();
}

void AgentManager::Update() {
    for (const auto& agentGroup : _agentGroups) {
        for (const auto& agent : agentGroup) {
            agent->CheckEating(_environment->getObject()->getPosition());
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
    for (const auto& agentGroup : _agentGroups) {
        if (std::find(agentGroup.begin(), agentGroup.end(), agent) != agentGroup.end()) {
            auto newGoal = _motionPlanner->GetRandomValidPoint();
            auto seed = _environment->GetClosestSeedTo(agent->getPosition());
            if (seed != nullptr) {
                newGoal = seed->position;
            }

            for (const auto& agentInGroup : agentGroup) {
                auto perturbedGoal = newGoal + Utils::RandomVector();
                while (_motionPlanner->_cSpace.PointIsInsideObstacle(perturbedGoal)) {
                    perturbedGoal = newGoal + Utils::RandomVector();
                }

                agentInGroup->SetGoal(perturbedGoal, seed);
            }
        }
    }
}

void AgentManager::InitializeAgents() {
    for (int i = 0; i < 6; i++) {
        _agentGroups.emplace_back();
        glm::vec3 groupGoal = _motionPlanner->GetRandomValidPoint();
        glm::vec3 groupStart = _motionPlanner->GetRandomValidPoint();
        auto color = Utils::RandomVector01();

        for (int j = 0; j < 9; j++) {
            auto newAgent = new Agent(groupStart, groupGoal, _motionPlanner, this);
            newAgent->SetColor(color);
            _agentGroups[i].push_back(newAgent);
        }
    }
}
