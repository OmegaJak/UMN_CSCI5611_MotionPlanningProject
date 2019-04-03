#include <algorithm>
#include <functional>
#include <iostream>
#include "KDTree.h"
#include "Utils.h"

const int K = 2;

using namespace std;
using namespace glm;

function<bool(const vec2&, const vec2&)> GetComparePoints(const int axis) {
    return [axis](const vec2& v1, const vec2& v2) { return v1[axis] < v2[axis]; };
}

KDTree::KDTree(vector<vec2> points) : KDTree(points, 0) {}

KDTree::KDTree(vector<vec2> points, int depth) {
    _root = CreateKDTree(points, depth);
}

KDNode* KDTree::CreateKDTree(vector<vec2> points, int depth) {
    if (points.empty()) return nullptr;

    int axis = depth % K;

    sort(points.begin(), points.end(), GetComparePoints(axis));
    Utils::PrintPoints(points);

    int medianIndex = points.size() / 2;
    auto median = points[medianIndex];

    auto medianIterator = points.begin() + medianIndex;
    if (points.size() == 1) {
        return new KDNode{median, nullptr, nullptr};
    }
    return new KDNode{median, CreateKDTree(std::vector<vec2>(points.begin(), medianIterator), depth + 1),
                      CreateKDTree(std::vector<vec2>(medianIterator + 1, points.end()), depth + 1)};
}
