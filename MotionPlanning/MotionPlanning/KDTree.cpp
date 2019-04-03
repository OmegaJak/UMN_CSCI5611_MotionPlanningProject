#include <algorithm>
#include <functional>
#include <iostream>
#include "KDTree.h"
#include "Utils.h"

using namespace std;
using namespace glm;

template <class T>
function<bool(const T&, const T&)> GetComparePoints(const int axis) {
    return [axis](const T& v1, const T& v2) { return v1[axis] < v2[axis]; };
}

template <class T>
KDTree<T>::KDTree(std::vector<T> points, int k) : KDTree(points, k, 0) {}

template <class T>
KDTree<T>::KDTree(std::vector<T> points, int k, int depth) {
    _k = k;
    _root = CreateKDTree(points, depth);
}

template <class T>
typename KDTree<T>::KDNode* KDTree<T>::CreateKDTree(std::vector<T> points, int depth) {
    if (points.empty()) return nullptr;

    int axis = depth % _k;

    sort(points.begin(), points.end(), GetComparePoints<T>(axis));
    Utils::PrintPoints(points);

    int medianIndex = points.size() / 2;
    auto median = points[medianIndex];

    auto medianIterator = points.begin() + medianIndex;
    if (points.size() == 1) {
        return new KDNode{median, nullptr, nullptr};
    }
    return new KDNode{median, CreateKDTree(std::vector<T>(points.begin(), medianIterator), depth + 1),
                      CreateKDTree(std::vector<T>(medianIterator + 1, points.end()), depth + 1)};
}

// Necessary because of weird C++ template linking stuff.
// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
template class KDTree<vec2>;
template class KDTree<vec3>;
