#pragma once
#include "GameObject.h"
#include "DebugManager.h"

struct Node {
	std::vector<Node*> connections;
	glm::vec3 position;
	bool explored;
};


class MotionPlanner {
   public:
   MotionPlanner();
   void Update();
   bool Solve(Node* current, Node* goal);
   bool GreedySolve(Node* current, Node* goal);
   int numsamples;
   std::vector<Node*> pbr;
   std::vector<Node*> solution;

   private:
    std::vector<GameObject> _gameObjects;
	
	Model* _sphereModel;

	void CreateMotionPlanner();
};

void Connect(Node* n1, Node* n2);
