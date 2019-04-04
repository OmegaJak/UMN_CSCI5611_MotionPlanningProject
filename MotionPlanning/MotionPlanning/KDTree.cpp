#include <algorithm>
#include "KDTree.h"
#include "Search.h"
#include "Utils.h"

using namespace std;
using namespace glm;

template <class nodeType, class vecType>
KDTree<nodeType, vecType>::KDTree(std::vector<nodeType> nodes, std::function<vecType(nodeType)> nodeToLocation, int k)
    : KDTree(nodes, nodeToLocation, k, 0) {}

template <class nodeType, class vecType>
KDTree<nodeType, vecType>::KDTree(std::vector<nodeType> nodes, std::function<vecType(nodeType)> nodeToLocation, int k, int depth) {
    _k = k;
    _nodeToLocation = nodeToLocation;

    _root = CreateKDTree(nodes, depth);
}

template <class nodeType, class vecType>
typename KDTree<nodeType, vecType>::KDNode* KDTree<nodeType, vecType>::CreateKDTree(vector<nodeType> nodes, int depth) {
    if (nodes.empty()) return nullptr;

    int axis = depth % _k;

    sort(nodes.begin(), nodes.end(),
         [axis, this](const nodeType& n1, const nodeType& n2) { return _nodeToLocation(n1)[axis] < _nodeToLocation(n2)[axis]; });
    // Utils::PrintPoints(nodes);

    int medianIndex = nodes.size() / 2;
    nodeType median = nodes[medianIndex];

    auto medianIterator = nodes.begin() + medianIndex;
    if (nodes.size() == 1) {
        return new KDNode{median, _nodeToLocation(median), nullptr, nullptr};
    }
    return new KDNode{median, _nodeToLocation(median), CreateKDTree(vector<nodeType>(nodes.begin(), medianIterator), depth + 1),
                      CreateKDTree(vector<nodeType>(medianIterator + 1, nodes.end()), depth + 1)};
}

template <class nodeType, class vecType>
vector<nodeType> KDTree<nodeType, vecType>::GetNearestNeighbors(const vecType& point, int n) {
    auto bestInit = vector<Candidate>(n, Candidate(nullptr, INFINITY));
    BestCandidateQueue best = BestCandidateQueue([](const Candidate& a, const Candidate& b) { return a.second < b.second; }, bestInit);

    // Calling this puts the results into the best queue
    GetNearestNeighbors(point, _root, best, 0);

    vector<nodeType> result = {};
    while (!best.empty()) {
        Candidate currentCandidate = best.top();
        best.pop();
        if (currentCandidate.first != nullptr) result.push_back(currentCandidate.first->nodeData);
    }

    return result;
}

template <class nodeType, class vecType>
void KDTree<nodeType, vecType>::GetNearestNeighbors(const vecType& point, KDNode* node, BestCandidateQueue& best, int depth) {
    if (node == nullptr) return;
    int axis = depth % _k;

    auto nodeAxisValue = node->location[axis];
    auto pointAxisValue = point[axis];

    // Traverse the tree
    if (pointAxisValue <= nodeAxisValue) {
        GetNearestNeighbors(point, node->left, best, depth + 1);
    } else {
        GetNearestNeighbors(point, node->right, best, depth + 1);
    }

    // Are we closer than the currently known closest? (and aren't at the same point)
    double dist = GetSqrDistanceBetween(point, node->location);
    if (dist > 0 && dist < best.top().second) {
        best.pop();
        best.push(Candidate(node, dist));
    }

    // We square distFromSplitPlane here because we avoided computing square roots for the distance computation above, and squaring
    // distFromSplitPlane is less work than sqrting dist
    auto distFromSplitPlane = abs(pointAxisValue - nodeAxisValue);
    if (distFromSplitPlane * distFromSplitPlane < best.top().second) {  // Closest could be on the other side of the split plane
        // So take the path we didn't take before
        if (pointAxisValue <= nodeAxisValue) {
            GetNearestNeighbors(point, node->right, best, depth + 1);
        } else {
            GetNearestNeighbors(point, node->left, best, depth + 1);
        }
    }
}

template <class nodeType, class vecType>
double KDTree<nodeType, vecType>::GetSqrDistanceBetween(const vecType& a, const vecType& b) {
    double sqrDistance = 0;
    for (int i = 0; i < _k; i++) {
        sqrDistance += pow(b[i] - a[i], 2);
    }

    return sqrDistance;
}

// Necessary because of weird C++ template linking stuff.
// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
template class KDTree<Node*, vec2>;
template class KDTree<Node*, vec3>;
