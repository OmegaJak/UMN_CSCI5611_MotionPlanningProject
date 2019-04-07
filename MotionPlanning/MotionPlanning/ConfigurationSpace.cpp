#include "ConfigurationSpace.h"
#include "Utils.h"

ConfigurationSpace::ConfigurationSpace(Obstacle* obstacle, Extents extents) : _extents(extents) {
    AddObstacle(obstacle);
}

ConfigurationSpace::ConfigurationSpace(std::vector<Obstacle*> obstacles, Extents extents) : _extents(extents) {
    for (auto obstacle : obstacles) {
        AddObstacle(obstacle);
    }
}

ConfigurationSpace::~ConfigurationSpace() {
    _obstacles.clear();
}

void ConfigurationSpace::AddObstacle(Obstacle* obstacle) {
    obstacle->InflateToCSpaceObstacleForSphericalAgent(_agentRadius);
    _obstacles.push_back(obstacle);
}

bool ConfigurationSpace::PointIsInsideObstacle(const glm::vec3& point) const {
    for (Obstacle* obstacle : _obstacles) {
        if (obstacle->IsPointInside(point)) return true;
    }

    return false;
}

bool ConfigurationSpace::SegmentIntersectsObstacle(const glm::vec3& segmentStart, const glm::vec3& segmentEnd) const {
    // This is necessary to ensure that the same two inputs, regardless of their order, will return the same result
    // Without doing this, it is possible (due to floating point unpleasantness) that the two different orders
    // that this can be called in would return different results. By always using the one with the smaller x,
    // so long as their x values differ, this will give the same result for the same pair or segment points.
    if (segmentStart.x < segmentEnd.x) {
        return SortedSegmentIntersectsObstacle(segmentStart, segmentEnd);
    } else {
        return SortedSegmentIntersectsObstacle(segmentEnd, segmentStart);
    }
}

glm::vec3 ConfigurationSpace::GetObstaclesRepulsionVelocity(const glm::vec3& point, float maxDistFromSurface) {
    glm::vec3 velocity = glm::vec3(0, 0, 0);
    for (Obstacle* obstacle : _obstacles) {
        auto dist = obstacle->GetDistanceFromPointToSurface(point);
        if (dist < 0.01) dist = 0.01;
        if (dist < maxDistFromSurface) {
            velocity += (obstacle->GetRepulsionNormalToPoint(point) / dist);
        }
    }

    return velocity;
}

glm::vec3 ConfigurationSpace::GetRandomValidPoint() const {
    glm::vec3 goal = Utils::RandomVector01();
    while (PointIsInsideObstacle(goal)) {
        goal = _extents.minExtent + Utils::RandomVector01() * glm::vec3(_extents.xLength, _extents.yLength, _extents.zLength);
    }

    return goal;
}

void ConfigurationSpace::Update() {
    for (Obstacle* obstacle : _obstacles) {
        obstacle->Update();
    }
}

bool ConfigurationSpace::SortedSegmentIntersectsObstacle(const glm::vec3& segmentStart, const glm::vec3& segmentEnd) const {
    for (Obstacle* obstacle : _obstacles) {
        if (obstacle->DoesSegmentIntersect(segmentStart, segmentEnd)) {
            return true;
        }
    }

    return false;
}
