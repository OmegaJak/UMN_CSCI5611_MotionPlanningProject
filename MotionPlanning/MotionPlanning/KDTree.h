#pragma once
#include <detail/type_vec2.hpp>
#include <detail/type_vec3.hpp>
#include <vector>

struct KDNode {
    glm::vec2 location;
    struct KDNode* left;
    struct KDNode* right;
};

// Much credit to https://en.wikipedia.org/wiki/K-d_tree
class KDTree {
   public:
    KDTree(std::vector<glm::vec2> points);
    KDTree(std::vector<glm::vec2> points, int depth);

   private:
    KDNode* CreateKDTree(std::vector<glm::vec2> points, int depth);

    KDNode* _root;
};
