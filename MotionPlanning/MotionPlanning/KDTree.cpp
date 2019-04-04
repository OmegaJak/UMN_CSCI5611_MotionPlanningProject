#include <algorithm>
#include "KDTree.h"
#include "Utils.h"

using namespace std;
using namespace glm;

template <class T>
KDTree<T>::KDTree(vector<T> points, int k) : KDTree(points, k, 0) {}

template <class T>
KDTree<T>::KDTree(vector<T> points, int k, int depth) {
    _k = k;
    _root = CreateKDTree(points, depth);
}

template <class T>
typename KDTree<T>::KDNode* KDTree<T>::CreateKDTree(vector<T> points, int depth) {
    if (points.empty()) return nullptr;

    int axis = depth % _k;

    sort(points.begin(), points.end(), [axis](const T& v1, const T& v2) { return v1[axis] < v2[axis]; });
    Utils::PrintPoints(points);

    int medianIndex = points.size() / 2;
    auto median = points[medianIndex];

    auto medianIterator = points.begin() + medianIndex;
    if (points.size() == 1) {
        return new KDNode{median, nullptr, nullptr};
    }
    return new KDNode{median, CreateKDTree(vector<T>(points.begin(), medianIterator), depth + 1),
                      CreateKDTree(vector<T>(medianIterator + 1, points.end()), depth + 1)};
}

template <class T>
vector<T> KDTree<T>::GetNearestNeighbors(const T& point, int n) {
    auto bestInit = vector<Candidate>(n, Candidate(nullptr, INFINITY));
    BestCandidateQueue best = BestCandidateQueue([](const Candidate& a, const Candidate& b) { return a.second < b.second; }, bestInit);

    // Calling this puts the results into the best queue
    GetNearestNeighbors(point, _root, best, 0);

    vector<T> result = {};
    while (!best.empty()) {
        Candidate currentCandidate = best.top();
        best.pop();
        if (currentCandidate.first != nullptr) result.push_back(currentCandidate.first->location);
    }

    return result;
}

template <class T>
void KDTree<T>::GetNearestNeighbors(const T& point, KDNode* node, BestCandidateQueue& best, int depth) {
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

    // Are we closer than the currently known closest?
    double dist = GetSqrDistanceBetween(point, node->location);
    if (dist < best.top().second) {
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

template <class T>
double KDTree<T>::GetSqrDistanceBetween(const T& a, const T& b) {
    double sqrDistance = 0;
    for (int i = 0; i < _k; i++) {
        sqrDistance += pow(b[i] - a[i], 2);
    }

    return sqrDistance;
}

// Necessary because of weird C++ template linking stuff.
// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
template class KDTree<vec2>;
template class KDTree<vec3>;
