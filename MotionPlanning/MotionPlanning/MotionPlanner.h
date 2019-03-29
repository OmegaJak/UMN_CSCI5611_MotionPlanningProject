#pragma once
#include "DebugManager.h"
#include "GameObject.h"
#include "Search.h"

class MotionPlanner {
   public:
    MotionPlanner();
    void Update();

    int numsamples;
    std::vector<Node*> pbr;
    std::vector<Node*> solution;

   private:
    std::vector<GameObject> _gameObjects;

    void CreateMotionPlanner();
    void Connect(Node* n1, Node* n2) const;
};
