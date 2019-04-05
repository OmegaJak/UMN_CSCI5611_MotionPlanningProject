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
    void Move(float velocity);
    void ChooseNewGoal();

    void InitializeStartAndGoal(const glm::vec3& startPosition, const glm::vec3& goalPosition);
    void PlanPath();

    Node* _start;
    Node* _goal;

    MotionPlanner* _motionPlanner;
    std::vector<Node*> _solutionPath;
    LineIndexRange _debugLines;
};
