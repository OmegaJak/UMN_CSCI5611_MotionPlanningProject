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
}

void MotionPlanner::CreateMotionPlanner() {
    const float elevation = 0.5f;
    GameObject gameObject;
    int currentNodeId = 0;

    // Begin PRM construction //
    Timer::StartTimer("PRMConstruction");

    Node* start = new Node();
    start->id = currentNodeId++;
    start->position = glm::vec3(-10, -10, -10);
    Node* end = new Node();
    end->id = currentNodeId++;
    end->position = glm::vec3(10, 10, 10);
    pbr.push_back(start);
    pbr.push_back(end);

    for (int i = 0; i < numsamples; i++) {
        Node* n = new Node();
        n->id = currentNodeId++;

        glm::vec3 pos = Utils::RandomVector() * 10.f;
        while (_cSpace.PointIsInsideObstacle(pos)) {  // Ensure we don't collide with any obstacles
            pos = Utils::RandomVector() * 10.f;
        }

        n->position = pos;
        pbr.push_back(n);

        gameObject = GameObject(ModelManager::SphereModel);  // PBR individual point
        gameObject.SetTextureIndex(UNTEXTURED);
        gameObject.SetColor(glm::vec3(.5f, .5f, .5f));
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
    if (Search::Solve(start, end, &solution)) {
        // printf("FOUND SOLUTION\n");
    } else {
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
        glm::vec3 dir = glm::normalize(solution[currentNode + 1]->position - solution[currentNode]->position);
        object->SetPosition(solution[currentNode]->position + distanceToTravel * dir);
    }
}
void MotionPlanner::MoveObjectSmooth(GameObject* object, float velocity, float dt) {
    glm::vec3 pos = object->getPosition();
    // If object is near the goal, don't move
    if (glm::distance(pos, solution.back()->position) < .1) {
        return;
    }
    // Find furthest visible point (fvp) along path that the object can see
    glm::vec3 fvp = getFurthestVisiblePoint(pos);
    glm::vec3 dir = glm::normalize(fvp - pos);

    // Move towards that point
    glm::vec3 newpos = pos + .1f * dir;
    object->SetPosition(newpos);
}

glm::vec3 MotionPlanner::getFurthestVisiblePoint(glm::vec3 pos) {
    int solutionsize = solution.size() - 1;
    for (int i = solutionsize; i >= 0; i--) {
        if (!(_cSpace.SegmentIntersectsObstacle(pos, solution[i]->position))) return solution[i]->position;
    }
    return pos;
}
