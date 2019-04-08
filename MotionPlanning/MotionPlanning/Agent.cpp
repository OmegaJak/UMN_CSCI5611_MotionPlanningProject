#include <map>
#include "Agent.h"
#include "AgentManager.h"
#include "Constants.h"
#include "ModelManager.h"

using namespace glm;

// Decent settings for agents without groups, picking their own goals
// Agent::EffectParams Agent::SeparationParams = {0.9f, 0.05f};
// Agent::EffectParams Agent::CohesionParams = {2.1f, 0.007f};
// Agent::EffectParams Agent::AlignmentParams = {3.5f, 0.2f};
// Agent::EffectParams Agent::ObstacleParams = {1.0f, 0.00015f};
// Agent::EffectParams Agent::PathParams = {0.13f, 0.2f};  // The first value is actually used as desired speed

// Decent settings for group navigation
Agent::EffectParams Agent::SeparationParams = {1.0f, 0.05f};
Agent::EffectParams Agent::CohesionParams = {2.0f, 0.003f};
Agent::EffectParams Agent::AlignmentParams = {4.0f, 0.3f};
Agent::EffectParams Agent::ObstacleParams = {0.75f, 0.001f};
Agent::EffectParams Agent::PathParams = {0.13f, 0.2f};  // The first value is actually used as desired speed

float Agent::Damping = 0.999;

Agent::Agent(const vec3& start, const vec3& goal, MotionPlanner* motionPlanner, AgentManager* agentManager)
    : GameObject(ModelManager::BirdModel) {
    SetTextureIndex(TEX0);

    _motionPlanner = motionPlanner;
    _agentManager = agentManager;

    InitializeVelocity();
    InitializeStartAndGoal(start, goal);

    // Get debug line indices
    _debugLines = DebugManager::RequestLines(3);
}

Agent::~Agent() {
    _solutionPath.clear();
}

void Agent::SetGoal(const vec3& newGoal, Seed* seed) {
    _seedGoal = seed;
    InitializeStartAndGoal(_position, newGoal);
}

void Agent::Update() {
    // FollowPath(0.1f);
    if (AgentManager::AgentsRunning && !eating) {
        _velocity += GetSeparationVelocity() + GetCohesionVelocity() + GetAlignmentVelocity() + GetObstacleAvoidanceVelocity() +
                     GetFollowPathVelocity();
        _velocity *= Damping;
        if (length(_velocity) > 1) {
            _velocity = normalize(_velocity);
        }
        Move();
    }
    GameObject::Update();
}

void Agent::Reset() {
    SetPosition(_start->position);
    InitializeVelocity();
}

void Agent::Move() {
    // Move towards that point
    vec3 newPos = _position + _velocity;
    if (newPos.x > 25) newPos.x = -25;
    if (newPos.x < -25) newPos.x = 25;
    if (newPos.y > 25) newPos.y = -25;
    if (newPos.y < -25) newPos.y = 25;
    if (newPos.z > 25) newPos.z = -25;
    if (newPos.z < -25) newPos.z = 25;

    SetPosition(newPos);

    DebugManager::SetLine(_debugLines.firstIndex + 2, _position, _position + _velocity * 5.0f, vec3(0.2, 0.2, 0.9));
}

vec3 Agent::GetSeparationVelocity() {
    auto toSeparateFrom = _agentManager->GetAllAgentsWithinDistanceOf(this, SeparationParams.radius);
    vec3 vel = vec3(0);
    for (auto other : toSeparateFrom) {
        vel += _position - other->_position;
    }

    return vel * SeparationParams.strength;
}

vec3 Agent::GetCohesionVelocity() {
    auto neighbors = _agentManager->GetAllAgentsWithinDistanceOf(this, CohesionParams.radius);

    vec3 positionTotal = vec3(0);
    if (neighbors.empty()) return positionTotal;

    for (auto neighbor : neighbors) {
        positionTotal += neighbor->_position;
    }

    vec3 averagePosition = positionTotal / float(neighbors.size());

    return (averagePosition - _position) * CohesionParams.strength;
}

vec3 Agent::GetAlignmentVelocity() {
    auto neighbors = _agentManager->GetAllAgentsWithinDistanceOf(this, AlignmentParams.radius);

    vec3 velocityTotal = vec3(0);
    if (neighbors.empty()) return velocityTotal;

    for (auto neighbor : neighbors) {
        velocityTotal += neighbor->_velocity;
    }

    vec3 averageVelocity = velocityTotal / float(neighbors.size());

    return (averageVelocity - _velocity) * AlignmentParams.strength;
}

vec3 Agent::GetObstacleAvoidanceVelocity() {
    auto velocity = _motionPlanner->GetObstaclesRepulsionVelocity(_position, ObstacleParams.radius);

    return velocity * ObstacleParams.strength;
}

vec3 Agent::GetFollowPathVelocity() {
    // If no solution was found, continue trying to find a new one while drifting
    if (_solutionPath.empty()) {
        InitializeStartAndGoal(_position, _goal->position);
        return vec3(0, 0, 0);
    }

    if (distance(_position, _solutionPath.back()->position) < _goalRadius) {
        if (_seedGoal != nullptr) {
            HandleSeedReached();
        } else {
            _agentManager->SetNewGroupGoal(this);
        }

        return GetFollowPathVelocity();
    }

    // Find furthest visible point (fvp) along path that the object can see
    vec3 vel = vec3(0, 0, 0);
    vec3 fvp = _motionPlanner->GetFarthestVisiblePointAlongPath(_position, _solutionPath);
    vec3 toCurrentGoal = fvp - _position;
    float distToCurrentGoal = length(toCurrentGoal);
    if (distToCurrentGoal == 0) {
        InitializeStartAndGoal(_position, _goal->position);
        return GetFollowPathVelocity();
    }

    toCurrentGoal = toCurrentGoal * PathParams.radius / distToCurrentGoal;
    vel = (toCurrentGoal - _velocity) * PathParams.strength;

    // Update the debug lines
    DebugManager::SetLine(_debugLines.firstIndex, _position, fvp, vec3(0, 1, 0));
    DebugManager::SetLine(_debugLines.firstIndex + 1, _position, _goal->position, vec3(0.9, 0.37, 0.1));

    return vel;
}

void Agent::InitializeStartAndGoal(const vec3& startPosition, const vec3& goalPosition) {
    bool didSomething = false;

    if (_start == nullptr || _start->position != startPosition) {
        delete _start;

        if (_position != startPosition) SetPosition(startPosition);
        _start = new Node();
        _start->position = _position;
        _start->connections = _motionPlanner->GetNNearestVisiblePoints(_position, PRM_CONNECTIONS_PER_NODE);
        didSomething = true;
    }

    if (_goal == nullptr || _goal->position != goalPosition) {
        delete _goal;

        _goal = new Node();
        _goal->position = goalPosition;
        _goal->connections = _motionPlanner->GetNNearestVisiblePoints(goalPosition, PRM_CONNECTIONS_PER_NODE);
        didSomething = true;
    }

    if (didSomething) PlanPath();
}

void Agent::InitializeVelocity() {
    _velocity = vec3(0, 1, 0) * 0.05f;
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

void Agent::HandleSeedReached() {
    printf("I ate a seed!");
    _agentManager->_environment->RemoveSeed(_seedGoal);
    _agentManager->SetNewGroupGoal(this);
    eating = true;
}

void Agent::CheckEating(glm::vec3 pos) {
    glm::vec3 pos2 = getPosition();

    //printf("Person is at: %f %f %f\n", pos.x, pos.y, pos.z);
    //printf("Bird is at: %f %f %f\n", pos2.x, pos2.y, pos2.z);
    if (glm::distance(glm::vec3(pos2.x, pos2.y, 0),glm::vec3(pos.x, pos.y, 0)) < 5) {
        eating = false;
	}
}