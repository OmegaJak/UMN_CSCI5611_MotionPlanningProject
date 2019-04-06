#include "Agent.h"
#include "Constants.h"
#include "ModelManager.h"

using namespace glm;

Agent::Agent(const vec3& start, const vec3& goal, MotionPlanner* motionPlanner) : GameObject(ModelManager::BirdModel) {
    SetTextureIndex(TEX0);

    _motionPlanner = motionPlanner;

    InitializeStartAndGoal(start, goal);
    PlanPath();

    // Get debug line indices
    _debugLines = DebugManager::RequestLines(2);
}

Agent::~Agent() {
    _solutionPath.clear();
}

void Agent::Update() {
    Move(0.1f);
    GameObject::Update();
}

void Agent::Move(float velocity) {
    // If no solution was found or we're near the goal, don't move
    if (_solutionPath.empty() || distance(position_, _solutionPath.back()->position) < .1) {
        ChooseNewGoal();
        return;
    }

    // Find furthest visible point (fvp) along path that the object can see
    vec3 fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(position_, _solutionPath);
    vec3 toCurrentGoal = fvp - position_;
    float distToCurrentGoal = length(toCurrentGoal);
    if (distToCurrentGoal < velocity) {
        SetPosition(fvp);
        velocity -= distToCurrentGoal;
        fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(fvp, _solutionPath);
        toCurrentGoal = fvp - position_;
        distToCurrentGoal = length(toCurrentGoal);

        if (distToCurrentGoal == 0) return;
    }

    toCurrentGoal /= distToCurrentGoal;

    // Move towards that point
    vec3 newPos = position_ + velocity * toCurrentGoal;
    SetPosition(newPos);

    // Update the debug line
    DebugManager::SetLine(_debugLines.firstIndex, position_, fvp, vec3(0, 1, 0));
    DebugManager::SetLine(_debugLines.firstIndex + 1, position_, _goal->position, vec3(0.9, 0.37, 0.1));
}

void Agent::ChooseNewGoal() {
    InitializeStartAndGoal(position_, _motionPlanner->GetRandomValidPoint());
    PlanPath();
}

void Agent::InitializeStartAndGoal(const vec3& startPosition, const vec3& goalPosition) {
    SetPosition(startPosition);
    _start = new Node();
    _start->position = position_;
    _start->connections = _motionPlanner->GetNNearestVisiblePoints(position_, PRM_CONNECTIONS_PER_NODE);

    _goal = new Node();
    _goal->position = goalPosition;
    _goal->connections = _motionPlanner->GetNNearestVisiblePoints(goalPosition, PRM_CONNECTIONS_PER_NODE);
}

void Agent::PlanPath() {
    // Temporarily add the goal to the graph
    for (Node* node : _goal->connections) {
        node->connections.push_back(_goal);
    }

    _solutionPath = _motionPlanner->PlanPath(_start, _goal);

    // After we've finished planning, remove the goal from the graph
    for (Node* node : _goal->connections) {
        node->connections.pop_back();
    }
}
