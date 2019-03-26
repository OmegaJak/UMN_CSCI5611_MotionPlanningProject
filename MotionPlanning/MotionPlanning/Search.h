#pragma once
#include <detail/type_vec3.hpp>
#include <vector>

struct Node {
    std::vector<Node*> connections;
    glm::vec3 position;
    bool explored;
};

class Search {
   public:
    static bool Solve(Node* current, Node* goal, std::vector<Node*>* solution);

   private:
    static bool GreedySolve(Node* current, Node* goal, std::vector<Node*>* solution);
};
