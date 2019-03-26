#include <gtc/type_ptr.hpp>
#include "Search.h"

bool Search::Solve(Node* current, Node* goal, std::vector<Node*>* solution) {
    return GreedySolve(current, goal, solution);
}

// Always goes to the node with the best heuristic. Not guaranteed to find a solution
// The solution is built in the solution variable that's passed in
bool Search::GreedySolve(Node* current, Node* goal, std::vector<Node*>* solution) {
    current->explored = true;
    if (current == goal) {
        solution->push_back(current);
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

    if (GreedySolve(current->connections[index], goal, solution)) {
        solution->push_back(current);
        return true;
    }

    return false;
}
