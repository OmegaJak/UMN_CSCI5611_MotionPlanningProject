#include <chrono>
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

    numsamples = 1000;
    CreateMotionPlanner();
    SetupDebugLines();
}

void MotionPlanner::CreateMotionPlanner() {
    const float elevation = 0.5f;
    int currentNodeId = 0;

    // Begin PRM construction //
    Timer::StartTimer("PRMConstruction");

    auto* start = new Node();
    start->position = vec3(-10, -10, -10);
    auto* end = new Node();
    end->position = vec3(10, 10, 10);
    pbr.push_back(start);
    pbr.push_back(end);

    for (int i = 0; i < numsamples; i++) {
        auto* n = new Node();

        vec3 pos = Utils::RandomVector() * 10.f;
        while (_cSpace.PointIsInsideObstacle(pos)) {  // Ensure we don't collide with any obstacles
            pos = Utils::RandomVector() * 10.f;
        }

        n->position = pos;
        pbr.push_back(n);

        GameObject gameObject = GameObject(ModelManager::SphereModel);  // PBR individual point
        gameObject.SetTextureIndex(UNTEXTURED);
        gameObject.SetColor(vec3(.5f, .5f, .5f));
        gameObject.SetPosition(n->position);
        gameObject.SetScale(.25, .25, .25);
        _gameObjects.push_back(gameObject);
    }

    Timer::StartTimer("KDTree Construction");
    auto kdTree = KDTree<Node*, vec3>(
        pbr, [](Node* n) { return n->position; }, 3);
    Timer::EndTimingAndPrintResult("KDTree Construction");

    // Make Valid Connections.
    for (int i = 0; i < pbr.size(); i++) {
        auto neighbors = kdTree.GetNearestNeighbors(pbr[i]->position, 5);
        for (auto& neighbor : neighbors) {
            Connect(pbr[i], neighbor);
        }

        /*for (int j = i + 1; j < pbr.size(); j++) {
            if (i == j) continue;

            Connect(pbr[i], pbr[j]);
        }*/
    }

    Timer::EndTimingAndPrintResult("PRMConstruction");
    // End PRM Construction //

    // Begin Solution Finding //
    Timer::StartTimer("Solution");

    if (start->connections.size() == 0) {
        printf("FAILED to find Solution");
        return;
    }
    if (!Search::Solve(start, end, &solution)) {
        printf("FAILED to find Solution");
    }

    Timer::EndTimingAndPrintResult("Solution");
    // End Solution Finding //
}

void MotionPlanner::Update() {
    _cSpace.Update();

    for (auto gameObject : _gameObjects) {
        gameObject.Update();
    }
}

void MotionPlanner::Connect(Node* n1, Node* n2) const {
    if (n1 != n2 &&
        glm::distance(n1->position, n2->position) < INFINITY) {  // Only allow connections that are somewhat close to each other.
        if (!_cSpace.SegmentIntersectsObstacle(n1->position, n2->position)) {
            n1->connections.push_back(n2);
            n2->connections.push_back(n1);
        }
    }
}

void MotionPlanner::MoveObject(GameObject* object, float speed, float time) {
    int currentNode = 0;
    int solutionsize = solution.size();
    float distToNextNode = 0;
    float distanceToTravel = speed * time;
    // This while loop finds out which node the "distance to Travel" falls between.
    while (distanceToTravel > 0 && currentNode < solutionsize - 1) {
        distToNextNode = glm::distance(solution[currentNode]->position, solution[currentNode + 1]->position);
        distanceToTravel -= distToNextNode;
        currentNode++;
    }
    currentNode--;  // Undo Last step of While loop (So distanceToTravel is not negative)
    distanceToTravel += distToNextNode;

    if (solutionsize > 0 && currentNode < solutionsize - 1) {
        vec3 dir = normalize(solution[currentNode + 1]->position - solution[currentNode]->position);
        object->SetPosition(solution[currentNode]->position + distanceToTravel * dir);
    }
}
void MotionPlanner::MoveObjectSmooth(GameObject* object, float velocity, float dt) {
    vec3 pos = object->getPosition();
    // If object is near the goal, don't move
    if (glm::distance(pos, solution.back()->position) < .1) {
        return;
    }
    // Find furthest visible point (fvp) along path that the object can see
    vec3 fvp = getFurthestVisiblePoint(pos);
    vec3 dir = normalize(fvp - pos);

    // Move towards that point
    vec3 newpos = pos + .1f * dir;
    object->SetPosition(newpos);
}

vec3 MotionPlanner::getFurthestVisiblePoint(vec3 pos) {
    int solutionsize = solution.size() - 1;
    for (int i = solutionsize; i >= 0; i--) {
        if (!(_cSpace.SegmentIntersectsObstacle(pos, solution[i]->position))) return solution[i]->position;
    }
    return pos;
}

void MotionPlanner::SetupDebugLines() {
    int numLines = 0;
    for (auto& node : pbr) {
        numLines += node->connections.size();
    }
    printf("Number of PRM edges: %d \n", numLines);

    // Render all PRM edges
    LineIndexRange lineIndices = DebugManager::RequestLines(numLines);
    int curline = lineIndices.firstIndex;
    for (auto n1 : pbr) {
        for (unsigned int j = 0; j < n1->connections.size(); j++) {
            Node* n2 = n1->connections[j];
            DebugManager::SetLine(curline, n1->position, n2->position, vec3(0, 0, 1));
            curline++;
        }
    }

    // Render solution path
    std::vector<Node*> solutions = solution;
    LineIndexRange lineIndices2 = DebugManager::RequestLines(solutions.size() - 1);
    for (unsigned int i = 1; i < solutions.size(); i++) {
        Node* n1 = solutions[i];
        Node* n2 = solutions[i - 1];
        DebugManager::SetLine(lineIndices.firstIndex + i - 1, n1->position, n2->position, vec3(0, 1, 0));
    }
}
