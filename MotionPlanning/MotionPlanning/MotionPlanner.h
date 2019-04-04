#pragma once
#include "ConfigurationSpace.h"
#include "DebugManager.h"
#include "GameObject.h"
#include "Search.h"

class MotionPlanner {
   public:
    explicit MotionPlanner(ConfigurationSpace cSpace);
    void Update();
    void MoveObject(GameObject* object, float speed, float dt);
    void MoveObjectSmooth(GameObject* object, float speed, float dt);

    int numsamples;
    std::vector<Node*> pbr;
    std::vector<Node*> solution;

   private:
    void CreateMotionPlanner();
    void Connect(Node* n1, Node* n2) const;
    glm::vec3 getFurthestVisiblePoint(glm::vec3 pos);
    void SetupDebugLines();

    std::vector<GameObject> _gameObjects;
    ConfigurationSpace _cSpace;
};
