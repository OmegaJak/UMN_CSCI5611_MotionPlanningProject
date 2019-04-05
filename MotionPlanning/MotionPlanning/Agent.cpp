#include "Agent.h"
#include "Constants.h"
#include "ModelManager.h"

using namespace glm;

Agent::Agent(const vec3& start, const vec3& goal, MotionPlanner* motionPlanner) : GameObject(ModelManager::BirdModel) {
    SetTextureIndex(TEX0);

    _motionPlanner = motionPlanner;

    _start = new Node();
    _start->position = start;
    _start->connections = _motionPlanner->GetNNearestVisiblePoints(start, PRM_CONNECTIONS_PER_NODE);
    SetPosition(start);

    _goal = new Node();
    _goal->position = goal;
    _goal->connections = _motionPlanner->GetNNearestVisiblePoints(goal, PRM_CONNECTIONS_PER_NODE);

    // Temporarily add the goal to the graph
    for (Node* node : _goal->connections) {
        node->connections.push_back(_goal);
    }

    _solutionPath = _motionPlanner->PlanPath(_start, _goal);

    // After we've finished planning, remove the goal from the graph
    for (Node* node : _goal->connections) {
        node->connections.pop_back();
    }

    // Get debug line indices
    _debugLines = DebugManager::RequestLines(1);
}

Agent::~Agent() {
    _solutionPath.clear();
}

void Agent::Update() {
    Move(0.1f);
    GameObject::Update();
}

void Agent::Move(const float velocity) {
    // If no solution was found or we're near the goal, don't move
    if (_solutionPath.empty() || distance(position_, _solutionPath.back()->position) < .1) {
        return;
    }

    // Find furthest visible point (fvp) along path that the object can see
    vec3 fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(position_, _solutionPath);
    vec3 toCurrentGoal = normalize(fvp - position_);

    // Move towards that point
    vec3 newPos = position_ + velocity * toCurrentGoal;
    SetPosition(newPos);

    // Update the debug line
    DebugManager::SetLine(_debugLines.firstIndex, newPos, fvp, vec3(1, 0, 0));
}
