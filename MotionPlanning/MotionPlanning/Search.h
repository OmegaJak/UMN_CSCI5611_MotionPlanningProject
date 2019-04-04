#pragma once
#include <detail/type_vec3.hpp>
#include <vector>

struct Node {
    struct AStarNodeData {
        AStarNodeData() = default;

        AStarNodeData(float gCost, float fCost, bool _opened) {
            g_cost = gCost;
            f_cost = fCost;
            opened = _opened;
        }

        float g_cost = INFINITY;
        float f_cost = INFINITY;
        Node* parent = nullptr;
        bool opened = false;
    };

    std::vector<Node*> connections;
    glm::vec3 position;
    bool explored;
    AStarNodeData aStarData;
};

class Search {
   public:
    static bool Solve(Node* start, Node* goal, const std::vector<Node*>& roadMap, std::vector<Node*>* solution);

   private:
    static bool GreedySolve(Node* current, Node* goal, std::vector<Node*>* solution);
    static bool A_Star(Node* start, Node* goal, const std::vector<Node*>& roadMap, std::vector<Node*>* solution);
    static std::vector<Node*> Reconstruct_Solution(Node* current);
};
