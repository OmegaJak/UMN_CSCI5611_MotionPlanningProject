#include <chrono>
#include "Constants.h"
#include "KDTree.h"
#include "ModelManager.h"
#include "MotionPlanner.h"
#include "Timer.h"
#include "Utils.h"

using namespace std;
using namespace std::chrono;
using namespace glm;

MotionPlanner::MotionPlanner(ConfigurationSpace cSpace) {
    _cSpace = cSpace;

    InitializePRM(500);
    SetupDebugLines();
}

MotionPlanner::~MotionPlanner() {
    _prm.clear();
}

std::vector<Node*> MotionPlanner::PlanPath(Node* start, Node* goal) const {
    Timer::StartTimer("Solution");

    auto solution = std::vector<Node*>();
    if (!Search::Solve(start, goal, _prm, &solution)) {
        printf("FAILED to find Solution\n");
    }

    Timer::StartTimer("SolutionCleanup");
    // Because we store A*'s relevant data in the nodes themselves, they must be reset after use
    // as multiple agent may search over this graph again
    for (auto& node : _prm) {
        node->aStarData = Node::AStarNodeData();
        node->explored = false;
    }
    Timer::EndTimingAndPrintResult("SolutionCleanup");

    Timer::EndTimingAndPrintResult("Solution");

    return solution;
}

void MotionPlanner::Update() {
    _cSpace.Update();

    if (DebugManager::ShouldRenderDebugLines) {
        for (auto gameObject : _gameObjects) {
            gameObject.Update();
        }
    }
}

vec3 MotionPlanner::GetFarthestVisiblePointAlongPath(const vec3& currentPos, const std::vector<Node*>& path) const {
    int solutionSize = path.size() - 1;
    for (int i = solutionSize; i >= 0; i--) {
        if (!_cSpace.SegmentIntersectsObstacle(currentPos, path[i]->position)) {
            return path[i]->position;
        }
    }

    return currentPos;
}

std::vector<Node*> MotionPlanner::GetNNearestVisiblePoints(const vec3& pos, int n) {
    return _prmKDTree.GetNearestNeighbors(pos, n,
                                          [this](const vec3& a, const vec3& b) { return !_cSpace.SegmentIntersectsObstacle(a, b); });
}

vec3 MotionPlanner::GetRandomValidPoint() const {
    return _cSpace.GetRandomValidPoint();
}

vec3 MotionPlanner::GetObstaclesRepulsionVelocity(const glm::vec3& point, float maxDistanceFromSurface) {
    return _cSpace.GetObstaclesRepulsionVelocity(point, maxDistanceFromSurface);
}

void MotionPlanner::InitializePRM(int numSamples) {
    Timer::StartTimer("PRMConstruction");

    for (int i = 0; i < numSamples; i++) {
        auto* n = new Node();

        n->position = GetRandomValidPoint();
        _prm.push_back(n);

        GameObject gameObject = GameObject(ModelManager::SphereModel);  // PBR individual point
        gameObject.SetTextureIndex(UNTEXTURED);
        gameObject.SetColor(vec3(.5f, .5f, .5f));
        gameObject.SetPosition(n->position);
        gameObject.SetScale(.25, .25, .25);
        _gameObjects.push_back(gameObject);
    }

    Timer::StartTimer("KDTree Construction");
    _prmKDTree = KDTree<Node*, vec3>(
        _prm, [](Node* n) { return n->position; }, 3);
    Timer::EndTimingAndPrintResult("KDTree Construction");

    // Make Valid Connections.
    for (int i = 0; i < _prm.size(); i++) {
        auto neighbors = GetNNearestVisiblePoints(_prm[i]->position, PRM_CONNECTIONS_PER_NODE);
        for (auto& neighbor : neighbors) {
            Connect(_prm[i], neighbor);
        }

        /*for (int j = i + 1; j < pbr.size(); j++) {
            if (i == j) continue;

            Connect(pbr[i], pbr[j]);
        }*/
    }

    Timer::EndTimingAndPrintResult("PRMConstruction");
}

void MotionPlanner::Connect(Node* n1, Node* n2) const {
    if (n1 != n2) {
        if (!_cSpace.SegmentIntersectsObstacle(n1->position, n2->position)) {
            n1->connections.push_back(n2);
            n2->connections.push_back(n1);
        }
    }
}

void MotionPlanner::SetupDebugLines() {
    int numLines = 0;
    for (auto& node : _prm) {
        numLines += node->connections.size();
    }
    printf("Number of PRM edges: %d \n", numLines);

    // Render all PRM edges
    LineIndexRange lineIndices = DebugManager::RequestLines(numLines);
    int curline = lineIndices.firstIndex;
    for (auto n1 : _prm) {
        for (unsigned int j = 0; j < n1->connections.size(); j++) {
            Node* n2 = n1->connections[j];
            DebugManager::SetLine(curline, n1->position, n2->position, vec3(0, 0, 1));
            curline++;
        }
    }

    // Render solution path
    /*std::vector<Node*> solutions = solution;
    LineIndexRange lineIndices2 = DebugManager::RequestLines(solutions.size() - 1);
    for (unsigned int i = 1; i < solutions.size(); i++) {
        Node* n1 = solutions[i];
        Node* n2 = solutions[i - 1];
        DebugManager::SetLine(lineIndices.firstIndex + i - 1, n1->position, n2->position, vec3(0, 1, 0));
    }*/
}
