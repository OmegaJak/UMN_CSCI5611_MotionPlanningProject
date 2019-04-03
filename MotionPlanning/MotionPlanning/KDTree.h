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
    std::vector<T> GetNearestNeighbors(const T& point, int n);

   private:
    KDNode* CreateKDTree(std::vector<T> points, int depth);
    std::vector<T> GetNearestNeighbors(const T& point, int n, KDNode* node, std::pair<KDNode*, double>& best, int depth);
    double GetSqrDistanceBetween(const T& a, const T& b);

    KDNode* _root;
    int _k{};
};
