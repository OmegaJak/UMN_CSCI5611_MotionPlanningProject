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
    if (_solutionPath.empty() || distance(_position, _solutionPath.back()->position) < .1) {
        ChooseNewGoal();
        return;
    }

    // Find furthest visible point (fvp) along path that the object can see
    vec3 fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(_position, _solutionPath);
    vec3 toCurrentGoal = fvp - _position;
    float distToCurrentGoal = length(toCurrentGoal);
    if (distToCurrentGoal < velocity) {
        SetPosition(fvp);
        velocity -= distToCurrentGoal;
        fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(fvp, _solutionPath);
        toCurrentGoal = fvp - _position;
        distToCurrentGoal = length(toCurrentGoal);

        if (distToCurrentGoal == 0) return;
    }

    toCurrentGoal /= distToCurrentGoal;

    // Move towards that point
    vec3 newPos = _position + velocity * toCurrentGoal;
    SetPosition(newPos);
    LookAt(fvp, vec3(0, -1, 0));

    // Update the debug line
    DebugManager::SetLine(_debugLines.firstIndex, _position, fvp, vec3(0, 1, 0));
    DebugManager::SetLine(_debugLines.firstIndex + 1, _position, _goal->position, vec3(0.9, 0.37, 0.1));
}

void Agent::ChooseNewGoal() {
    InitializeStartAndGoal(_position, _motionPlanner->GetRandomValidPoint());
    PlanPath();
}

void Agent::InitializeStartAndGoal(const vec3& startPosition, const vec3& goalPosition) {
    SetPosition(startPosition);
    _start = new Node();
    _start->position = _position;
    _start->connections = _motionPlanner->GetNNearestVisiblePoints(_position, PRM_CONNECTIONS_PER_NODE);

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
