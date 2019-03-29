#include <gtc/type_ptr.hpp>
#include <queue>
#include <set>
#include <unordered_set>
#include "Search.h"

bool Search::Solve(Node* start, Node* goal, std::vector<Node*>* solution) {
    return A_Star(start, goal, solution);
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

float Distance(Node* a, Node* b) {
    return glm::distance(a->position, b->position);
}

float Heuristic(Node* a, Node* b) {
    return Distance(a, b);
}

// This is closely based on the pseudocode here: https://en.wikipedia.org/wiki/A*_search_algorithm
bool Search::A_Star(Node* start, Node* goal, std::vector<Node*>* solution) {
    // Defining this here is questionable
    class NodeCompare {
       public:
        bool operator()(Node* a, Node* b) const {
            return a->aStarData.f_cost > b->aStarData.f_cost;
        }
    };

    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> fringe;

    start->aStarData = Node::AStarNodeData(0, Heuristic(start, goal), true);
    fringe.push(start);
	

    while (!fringe.empty()) {
        auto current = fringe.top();

        if (current == goal) {
            *solution = Reconstruct_Solution(current);
            return true;
        }
		printf("FRINGE SIZE %d \n", fringe.size());
        fringe.pop();
        current->explored = true;
        for (auto neighbor : current->connections) {
            if (neighbor->explored) {
                continue;
            }

            // Distance from start to neighbor
            auto tentativeGCost = current->aStarData.g_cost + Distance(current, neighbor);
            if (!neighbor->aStarData.opened) {
                fringe.push(neighbor);
                neighbor->aStarData.opened = true;
            } else if (tentativeGCost >= neighbor->aStarData.g_cost) {
                continue;
            }

            // This is the best path we've found so far
            neighbor->aStarData.parent = current;
            neighbor->aStarData.g_cost = tentativeGCost;
            neighbor->aStarData.f_cost = tentativeGCost + Heuristic(neighbor, goal);
        }
    }
	printf("PEE2");
    return false;
}

// Given the goal node at the end of A*, reconstruct the solution path
std::vector<Node*> Search::Reconstruct_Solution(Node* current) {
    auto solution = std::vector<Node*>();
    solution.push_back(current);
    while (current->aStarData.parent != nullptr) {
        current = current->aStarData.parent;
        solution.push_back(current);
    }

    return solution;  // Should this be reversed?
}
