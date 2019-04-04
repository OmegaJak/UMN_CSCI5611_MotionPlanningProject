#include "ConfigurationSpace.h"

ConfigurationSpace::ConfigurationSpace(Obstacle* obstacle) {
    AddObstacle(obstacle);
}

ConfigurationSpace::ConfigurationSpace(std::vector<Obstacle*> obstacles) {
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
    for (Obstacle* obstacle : _obstacles) {
        if (obstacle->DoesSegmentIntersect(segmentStart, segmentEnd)) return true;
    }

    return false;
}

void ConfigurationSpace::Update() {
    for (Obstacle* obstacle : _obstacles) {
        obstacle->Update();
    }
}
