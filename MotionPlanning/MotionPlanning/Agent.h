#pragma once
#include "GameObject.h"
#include "MotionPlanner.h"
#include "Search.h"

class Agent : GameObject {
   public:
    Agent(const glm::vec3& start, const glm::vec3& goal, MotionPlanner* motionPlanner);
    ~Agent();

    void Update() override;

   private:
    void Move(const float velocity);

    Node* _start;
    Node* _goal;

    MotionPlanner* _motionPlanner;
    std::vector<Node*> _solutionPath;
};
