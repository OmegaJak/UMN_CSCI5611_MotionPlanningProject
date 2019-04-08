#pragma once
#include "GameObject.h"
#include "MotionPlanner.h"
#include "Search.h"

class AgentManager;

class Agent : public GameObject {
   public:
    struct EffectParams {
        float radius;
        float strength;
    };

    static EffectParams SeparationParams;
    static EffectParams CohesionParams;
    static EffectParams AlignmentParams;
    static EffectParams ObstacleParams;
    static EffectParams PathParams;
    static float Damping;

    Agent(const glm::vec3& start, const glm::vec3& goal, MotionPlanner* motionPlanner, AgentManager* agentManager);
    ~Agent();

    void Update() override;
    void Reset();

   private:
    void Move();
    void ChooseNewGoal();

    glm::vec3 GetSeparationVelocity();
    glm::vec3 GetCohesionVelocity();
    glm::vec3 GetAlignmentVelocity();
    glm::vec3 GetObstacleAvoidanceVelocity();
    glm::vec3 GetFollowPathVelocity();

    void InitializeStartAndGoal(const glm::vec3& startPosition, const glm::vec3& goalPosition);
    void InitializeVelocity();
    void PlanPath();

    Node* _start = nullptr;
    Node* _goal = nullptr;
    float _goalRadius = 0.25f;

    MotionPlanner* _motionPlanner;
    AgentManager* _agentManager;
    std::vector<Node*> _solutionPath;
    LineIndexRange _debugLines;

    glm::vec3 _velocity;
};
