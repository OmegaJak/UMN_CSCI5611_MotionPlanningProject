#include "ModelManager.h"
#include "SphereObstacle.h"

SphereObstacle::SphereObstacle(const glm::vec3& position, float radius) {
    _environmentObstacle = GameObject(ModelManager::SphereModel);
    _environmentObstacle.SetTextureIndex(UNTEXTURED);
    _environmentObstacle.SetColor(glm::vec3(.5f, .2f, .3f));
    _environmentObstacle.SetPosition(position);
    _environmentObstacle.SetScale(radius * 2.f, radius * 2.f, radius * 2.f);

    _worldRadius = _cSpaceRadius = radius;
    _center = position;
}

void SphereObstacle::InflateToCSpaceObstacleForSphericalAgent(float agentRadius) {
    _cSpaceRadius = _worldRadius + agentRadius;
}

bool SphereObstacle::IsPointInside(glm::vec3 point) const {
    return glm::distance(point, _center) <= _cSpaceRadius;
}

bool SphereObstacle::DoesSegmentIntersect(glm::vec3 segmentStart, glm::vec3 segmentEnd) const {
    return SegmentSphereIntersect(segmentStart, segmentEnd, _center, _cSpaceRadius);
}

float SphereObstacle::GetDistanceFromPointToSurface(const glm::vec3& point) const {
    auto toPointFromCenter = point - _center;
    auto len = glm::length(toPointFromCenter);
    if (len > _cSpaceRadius) {
        return len - _cSpaceRadius;
    }

    return 0;
}

glm::vec3 SphereObstacle::GetRepulsionNormalToPoint(const glm::vec3& point) const {
    return glm::normalize(point - _center);
}

bool SphereObstacle::SegmentSphereIntersect(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& sphere_pos,
                                            const float sphere_radius) {
    glm::vec3 d = (p2 - p1);
    float lineLength = glm::length(d);
    d = d / lineLength;
    // Substitute Segment x,y,z into Sphere 3d equation, and solve for t. (Quadratic formula required)

    // if the direction was not normalized, it would look like this: float A = d.x  * d.x + d.y * d.y + d.z * d.z or
    // d.Dot(d); but since it is normalized, A is equal to 1

    // Quadratic formula values:
    glm::vec3 offset = p1 - sphere_pos;  // Offset sphere position from origin of segment (v is position of sphere, p is origin of segment)
    float B = 2 * glm::dot(offset, d);
    float C = glm::dot(offset, offset) - sphere_radius * sphere_radius;
    float disc = B * B - 4 * C;
    if (disc < 0) {  // if discriminant is less than 0, the segment does not intersect the sphere.
        return false;
    }
    float sqDisc = sqrt(disc);  // calculate square root of disc once to increase performance

    float t0 = (-B + sqDisc) / 2.f;  // t0 and t1 are the two points of intersection of a ray starting at the segment's origin.
    float t1 = (-B - sqDisc) / 2.f;
    return ((t0 >= 0 && t0 <= lineLength) ||
            (t1 >= 0 && t1 <= lineLength));  // If either t0 or t1 lies upon the line segment, there was an intersection
}
