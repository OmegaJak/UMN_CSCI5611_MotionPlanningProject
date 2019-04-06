#pragma once
#include <detail/type_mat.hpp>
#include "ConfigurationSpace.h"
#include "DebugManager.h"
#include "GameObject.h"
#include "KDTree.h"
#include "Search.h"

class MotionPlanner {
   public:
    explicit MotionPlanner(ConfigurationSpace cSpace);
    ~MotionPlanner();

    std::vector<Node*> PlanPath(Node* start, Node* goal) const;
    glm::vec3 GetFarthestVisiblePointAlongPath(const glm::vec3& currentPos, const std::vector<Node*>& path) const;
    std::vector<Node*> GetNNearestVisiblePoints(const glm::vec3& pos, int n);
    glm::vec3 GetRandomValidPoint() const;

    void Update();

   private:
    void InitializePRM(int numSamples);
    void Connect(Node* n1, Node* n2) const;
    void SetupDebugLines();

    std::vector<Node*> _prm;
    KDTree<Node*, glm::vec3> _prmKDTree;
    std::vector<GameObject> _gameObjects;
    ConfigurationSpace _cSpace;
};
