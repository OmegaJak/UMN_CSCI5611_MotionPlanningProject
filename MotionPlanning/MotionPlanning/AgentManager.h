#pragma once
#include <vector>
#include "Agent.h"
#include "Environment.h"

class AgentManager {
   public:
    static bool AgentsRunning;

    explicit AgentManager(MotionPlanner* motionPlanner, Environment* environment);
    void Update();
    void Reset();

    std::vector<Agent*> GetAllAgentsWithinDistanceOf(Agent* me, float distance);
    void SetNewGroupGoal(Agent* agent);

    Environment* _environment;

   private:
    void InitializeAgents();

    std::vector<std::vector<Agent*>> _agentGroups;
    MotionPlanner* _motionPlanner;
};
