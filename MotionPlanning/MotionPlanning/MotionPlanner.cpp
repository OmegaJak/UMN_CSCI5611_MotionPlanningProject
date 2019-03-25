#include "ClothManager.h"
#include "MotionPlanner.h"
#include "Utils.h"

MotionPlanner::MotionPlanner() {
    numsamples = 100;
    _sphereModel = new Model("models/sphere.txt");
    CreateMotionPlanner();
}

void MotionPlanner::CreateMotionPlanner() {
    const float elevation = 0.5f;
    GameObject gameObject;

    Node* start = new Node();
    start->position = glm::vec3(-10, -10, elevation);
    Node* end = new Node();
    end->position = glm::vec3(10, 10, elevation);
    pbr.push_back(start);
    pbr.push_back(end);

    for (int i = 0; i < numsamples; i++) {
        Node* n = new Node();

        glm::vec3 pos = glm::vec3(0);                                                 // Hard coded radius of 5, center of (0, 0, 0)
        while (glm::distance(glm::vec3(pos.x, pos.y, 0), glm::vec3(0, 0, 0)) <= 5) {  // Make sure points don't collide with center sphere
            pos = Utils::RandomVector() * 10.f;
            pos.z = elevation;
        }

        n->position = pos;
        n->position.z = elevation;
        pbr.push_back(n);

        gameObject = GameObject(_sphereModel);  // PBR individual point
        gameObject.SetTextureIndex(UNTEXTURED);
        gameObject.SetColor(glm::vec3(.5f, .5f, .5f));
        gameObject.SetPosition(n->position);
        gameObject.SetScale(.25, .25, .25);
        _gameObjects.push_back(gameObject);
    }

    // Make Valid Connections.
    for (int i = 0; i < pbr.size(); i++) {
        for (int j = 0; j < pbr.size(); j++) {
            if (i == j) continue;

            Connect(pbr[i], pbr[j]);
        }
    }

    // Depth First Search
    if (start->connections.size() == 0) {
        printf("FAILED to find Solution");
        return;
    }

    if (GreedySolve(start, end)) {
        solution.push_back(start);
        printf("FOUND SOLUTION\n");
    } else
        printf("FAILED to find Solution");
}

void MotionPlanner::Update() {
    for (auto gameObject : _gameObjects) {
        gameObject.Update();
    }
}

void Connect(Node* n1, Node* n2) {
    if (n1 != n2 && glm::distance(n1->position, n2->position) < 5) {  // Only allow connections that are somewhat close to each other.
        if (!(Utils::SegmentSphereIntersect(n1->position, n2->position, glm::vec3(0, 0, 0), 5))) {
            n1->connections.push_back(n2);
            n2->connections.push_back(n1);
        }
    }
}

// Regular Solve uses Depth First search to find the goal. Almost never will return an optimal route.
bool MotionPlanner::Solve(Node* current, Node* goal) {
    current->explored = true;

    if (current->position == goal->position) {
        solution.push_back(current);
        return true;
    }

    for (int i = 0; i < current->connections.size(); i++) {
        if (current->connections[i]->explored == false) {
            if (Solve(current->connections[i], goal)) {
                solution.push_back(current->connections[i]);
                return true;
            }
        }
    }

    return false;
}

// Always goes to the node with the best heuristic. Not guaranteed to find a solution
bool MotionPlanner::GreedySolve(Node* current, Node* goal) {
    current->explored = true;
    if (current == goal) {
        solution.push_back(current);
        return true;
    }

    float min = INFINITY;
    int index = -1;
    for (int i = 0; i < current->connections.size(); i++) {
        if (!current->connections[i]->explored && glm::distance(current->connections[i]->position, goal->position) < min) {
            min = glm::distance(current->connections[i]->position, goal->position);
            index = i;
        }
    }
    if (index < 0) return false;

    if (GreedySolve(current->connections[index], goal)) {
        solution.push_back(current);
        return true;
    }

    return false;
}
