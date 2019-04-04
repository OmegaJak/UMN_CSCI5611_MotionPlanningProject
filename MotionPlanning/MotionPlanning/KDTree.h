#pragma once
#include <functional>
#include <queue>
#include <vector>

// Much credit to https://en.wikipedia.org/wiki/K-d_tree
template <class nodeType, class vecType>
class KDTree {
   public:
    // Constructs a KD tree, where K is the number of dimensions the tree exists in
    KDTree(std::vector<nodeType> nodes, std::function<vecType(nodeType)> nodeToLocation, int k);
    KDTree(std::vector<nodeType> nodes, std::function<vecType(nodeType)> nodeToLocation, int k, int depth);

    // Gets the n nearest neighbors in the KD tree to the given point.
    // If n is less than K, this will only return a vector of size k
    std::vector<nodeType> GetNearestNeighbors(const vecType& point, int n);

   private:
    struct KDNode {
        nodeType nodeData;
        vecType location;
        struct KDNode* left;
        struct KDNode* right;
    };
    typedef std::pair<KDNode*, double> Candidate;
    typedef std::function<bool(Candidate, Candidate)> CandidateComparePred;
    typedef std::priority_queue<Candidate, std::vector<Candidate>, CandidateComparePred> BestCandidateQueue;

    KDNode* CreateKDTree(std::vector<nodeType> nodes, int depth);
    void GetNearestNeighbors(const vecType& point, KDNode* node, BestCandidateQueue& best, int depth);
    double GetSqrDistanceBetween(const vecType& a, const vecType& b);

    KDNode* _root;
    int _k;
    std::function<vecType(nodeType)> _nodeToLocation;
};
