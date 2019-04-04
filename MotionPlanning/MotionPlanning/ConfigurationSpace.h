#pragma once
#include <vector>
#include "Obstacle.h"

class ConfigurationSpace {
   public:
    ConfigurationSpace() = default;
    explicit ConfigurationSpace(Obstacle* obstacle);
    explicit ConfigurationSpace(std::vector<Obstacle*> obstacles);
    ~ConfigurationSpace();

    void AddObstacle(Obstacle* obstacle);
    bool PointIsInsideObstacle(const glm::vec3& point) const;
    bool SegmentIntersectsObstacle(const glm::vec3& segmentStart, const glm::vec3& segmentEnd) const;

    void Update();

   private:
    std::vector<Obstacle*> _obstacles;
    float _agentRadius = 1;
};
