#include "ClothManager.h"
#include "ModelManager.h"
#include "MotionPlanner.h"
#include "Utils.h"

MotionPlanner::MotionPlanner() {
    numsamples = 100;
    CreateMotionPlanner();
}

void MotionPlanner::CreateMotionPlanner() {
    const float elevation = 0.5f;
    GameObject gameObject;
    int currentNodeId = 0;

    Node* start = new Node();
    start->id = currentNodeId++;
    start->position = glm::vec3(-10, -10, elevation);
    Node* end = new Node();
    end->id = currentNodeId++;
    end->position = glm::vec3(10, 10, elevation);
    pbr.push_back(start);
    pbr.push_back(end);

    for (int i = 0; i < numsamples; i++) {
        Node* n = new Node();
        n->id = currentNodeId++;

        glm::vec3 pos = glm::vec3(0);                                                   // Hard coded radius of 5, center of (0, 0, 0)
        while (glm::distance(glm::vec3(pos.x, pos.y, 0), glm::vec3(0, 0, 0)) <= 2.5) {  // Make sure points don't collide with center sphere
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

    // Make Valid Connections.
    for (int i = 0; i < pbr.size(); i++) {
        for (int j = i + 1; j < pbr.size(); j++) {
            if (i == j) continue;

            Connect(pbr[i], pbr[j]);
        }
    }

    // Depth First Search
    if (start->connections.size() == 0) {
        printf("FAILED to find Solution");
        return;
    }
    if (Search::Solve(start, end, &solution)) {
        printf("FOUND SOLUTION\n");
    } else {
        printf("FAILED to find Solution");
    }
}

void MotionPlanner::Update() {
    for (auto gameObject : _gameObjects) {
        gameObject.Update();
    }
}

void MotionPlanner::Connect(Node* n1, Node* n2) const {
    if (n1 != n2 && glm::distance(n1->position, n2->position) < 10) {  // Only allow connections that are somewhat close to each other.
        if (!(Utils::SegmentSphereIntersect(n1->position, n2->position, glm::vec3(0, 0, 0), 2.5))) {
            n1->connections.push_back(n2);
            n2->connections.push_back(n1);
        }
    }
}
