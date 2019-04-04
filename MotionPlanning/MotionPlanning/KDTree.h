#pragma once
#include <functional>
#include <queue>
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
    typedef std::pair<KDNode*, double> Candidate;
    typedef std::function<bool(Candidate, Candidate)> CandidateComparePred;
    typedef std::priority_queue<Candidate, std::vector<Candidate>, CandidateComparePred> BestCandidateQueue;

    KDNode* CreateKDTree(std::vector<T> points, int depth);
    std::vector<T> GetNearestNeighbors(const T& point, KDNode* node, BestCandidateQueue& best, int depth);
    double GetSqrDistanceBetween(const T& a, const T& b);

    KDNode* _root;
    int _k{};
};
