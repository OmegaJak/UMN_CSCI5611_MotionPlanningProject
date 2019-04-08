#include <algorithm>
#include "BoxObstacle.h"
#include "ModelManager.h"

AABoxObstacle::AABoxObstacle(const glm::vec3& center, float xLength, float yLength, float zLength) {
    _center = center;
    _halfSideLength = glm::vec3(xLength, yLength, zLength) / 2.0f;
    _min = _center - _halfSideLength;
    _max = _center + _halfSideLength;
    InitRendering();
}

AABoxObstacle::AABoxObstacle(const glm::vec3& min, const glm::vec3& max) : _min(min), _max(max) {
    _halfSideLength = (max - min) / 2.0f;
    _center = min + _halfSideLength;
    InitRendering();
}

void AABoxObstacle::InflateToCSpaceObstacleForSphericalAgent(float agentRadius) {
    // Technically the corners should be rounded here but meh, this approximation is fine
    auto cornerShift = glm::vec3(agentRadius, agentRadius, agentRadius);
    _min -= cornerShift;
    _max += cornerShift;
    _halfSideLength = (_max - _min) / 2.0f;
}

bool AABoxObstacle::IsPointInside(glm::vec3 point) const {
    for (int axis = 0; axis < 3; axis++) {
        if (point[axis] < _min[axis] || point[axis] > _max[axis]) return false;
    }

    return true;
}

// https://stackoverflow.com/questions/3106666/intersection-of-line-segment-with-axis-aligned-box-in-c-sharp#3115514
bool AABoxObstacle::DoesSegmentIntersect(glm::vec3 segmentStart, glm::vec3 segmentEnd) const {
    auto beginToEnd = segmentEnd - segmentStart;
    auto beginToMin = _min - segmentStart;
    auto beginToMax = _max - segmentStart;
    for (int axis = 0; axis < 3; axis++) {
        if (beginToEnd[axis] == 0) {                             // Segment is axis-aligned
            if (beginToMin[axis] > 0 || beginToMax[axis] < 0) {  // If min is 'above' begin or max is 'below' end
                return false;                                    // The segment is not between the planes of this axis
            }
        } else {
            float t1 = beginToMin[axis] / beginToEnd[axis];
            float t2 = beginToMax[axis] / beginToEnd[axis];
            if ((t1 > 1 && t2 > 1) || (t1 < 0 && t2 < 0)) {  // If both intersection points are after the line end or before the line
                return false;                                // The segment does not cross any planes of this axis
            }
        }
    }
    return true;
}

// https://www.alanzucconi.com/2016/07/01/signed-distance-functions/
float AABoxObstacle::GetDistanceFromPointToSurface(const glm::vec3& point) const {
    if (IsPointInside(point)) {
        return 0;
    }

    // return sqrt(sum);
    float x = std::max(point.x - _center.x - _halfSideLength.x, _center.x - point.x - _halfSideLength.x);
    float y = std::max(point.y - _center.y - _halfSideLength.y, _center.y - point.y - _halfSideLength.y);
    float z = std::max(point.z - _center.z - _halfSideLength.z, _center.z - point.z - _halfSideLength.z);

    float d = x;
    if (y > d) d = y;
    if (z > d) d = z;
    return d;
}

glm::vec3 AABoxObstacle::GetRepulsionNormalToPoint(const glm::vec3& point) const {
    return glm::normalize(point - _center);
}

void AABoxObstacle::InitRendering() {
    _environmentObstacle = GameObject(ModelManager::CubeModel);
    _environmentObstacle.SetTextureIndex(UNTEXTURED);
    _environmentObstacle.SetColor(glm::vec3(.5f, .2f, .3f));
    _environmentObstacle.SetPosition(_center);
    _environmentObstacle.SetScale(_halfSideLength * 2.f);
}
