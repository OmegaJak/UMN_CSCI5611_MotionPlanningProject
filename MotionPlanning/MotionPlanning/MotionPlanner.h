#pragma once
#include "ConfigurationSpace.h"
#include "DebugManager.h"
#include "GameObject.h"
#include "Search.h"

class MotionPlanner {
   public:
    explicit MotionPlanner(ConfigurationSpace cSpace);
    void Update();

    int numsamples;
    std::vector<Node*> pbr;
    std::vector<Node*> solution;

   private:
    void CreateMotionPlanner();
    void Connect(Node* n1, Node* n2) const;

    std::vector<GameObject> _gameObjects;
    ConfigurationSpace _cSpace;
};
