#pragma once
#include <vector>

// Much credit to https://en.wikipedia.org/wiki/K-d_tree
template <class T>
class KDTree {
   public:
    struct KDNode {
        T location;
        struct KDNode* left;
        struct KDNode* right;
    };

    KDTree(std::vector<T> points, int k);
    KDTree(std::vector<T> points, int k, int depth);

   private:
    KDNode* CreateKDTree(std::vector<T> points, int depth);

    KDNode* _root;
    int _k;
};
