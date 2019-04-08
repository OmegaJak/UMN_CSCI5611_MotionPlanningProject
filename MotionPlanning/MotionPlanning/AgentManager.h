#pragma once
#include <vector>
#include "Agent.h"

class AgentManager {
   public:
    static bool AgentsRunning;

    explicit AgentManager(MotionPlanner* motionPlanner);
    void Update();
    void Reset();

    std::vector<Agent*> GetAllAgentsWithinDistanceOf(Agent* me, float distance);
    void SetNewGroupGoal(Agent* agent);

   private:
    void InitializeAgents();

    std::vector<std::vector<Agent*>> _agentGroups;
    MotionPlanner* _motionPlanner;
};
