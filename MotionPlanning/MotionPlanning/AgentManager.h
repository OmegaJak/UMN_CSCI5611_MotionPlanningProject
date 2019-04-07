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

   private:
    void InitializeAgents();

    std::vector<Agent*> _agents;
    MotionPlanner* _motionPlanner;
};
