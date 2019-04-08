#pragma once
#include "Environment.h"
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
    void CheckEating(glm::vec3 pos);
    Agent(const glm::vec3& start, const glm::vec3& goal, MotionPlanner* motionPlanner, AgentManager* agentManager);
    ~Agent();

    void SetGoal(const glm::vec3& newGoal, Seed* seed = nullptr);

    void Update() override;
    void Reset();

    float _goalRadius = 0.5f;

   private:
    void Move();
    bool eating;
    glm::vec3 GetSeparationVelocity();
    glm::vec3 GetCohesionVelocity();
    glm::vec3 GetAlignmentVelocity();
    glm::vec3 GetObstacleAvoidanceVelocity();
    glm::vec3 GetFollowPathVelocity();

    void InitializeStartAndGoal(const glm::vec3& startPosition, const glm::vec3& goalPosition);
    void InitializeVelocity();
    void PlanPath();
    void HandleSeedReached();

    Node* _start = nullptr;
    Node* _goal = nullptr;

    MotionPlanner* _motionPlanner;
    AgentManager* _agentManager;
    std::vector<Node*> _solutionPath;
    LineIndexRange _debugLines;

    glm::vec3 _velocity;
    Seed* _seedGoal;
};
