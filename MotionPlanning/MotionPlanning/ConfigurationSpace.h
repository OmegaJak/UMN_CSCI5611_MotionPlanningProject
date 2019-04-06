#pragma once
#include <algorithm>
#include <vector>
#include "Obstacle.h"

struct Extents {
    Extents() : Extents(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)) {}

    Extents(const glm::vec3& _minExtent, const glm::vec3& _maxExtent) {
        minExtent =
            glm::vec3(std::min(_minExtent.x, _maxExtent.x), std::min(_minExtent.y, _maxExtent.y), std::min(_minExtent.z, _maxExtent.z));
        maxExtent =
            glm::vec3(std::max(_minExtent.x, _maxExtent.x), std::max(_minExtent.y, _maxExtent.y), std::max(_minExtent.z, _maxExtent.z));

        xLength = maxExtent.x - minExtent.x;
        yLength = maxExtent.y - minExtent.y;
        zLength = maxExtent.z - minExtent.z;
    }

    glm::vec3 minExtent;
    glm::vec3 maxExtent;

    float xLength, yLength, zLength;
};

class ConfigurationSpace {
   public:
    ConfigurationSpace() = default;
    explicit ConfigurationSpace(Obstacle* obstacle, Extents extents);
    explicit ConfigurationSpace(std::vector<Obstacle*> obstacles, Extents extents);
    ~ConfigurationSpace();

    void AddObstacle(Obstacle* obstacle);
    bool PointIsInsideObstacle(const glm::vec3& point) const;
    bool SegmentIntersectsObstacle(const glm::vec3& segmentStart, const glm::vec3& segmentEnd) const;
    glm::vec3 GetRandomValidPoint() const;

    void Update();

   private:
    bool SortedSegmentIntersectsObstacle(const glm::vec3& segmentStart, const glm::vec3& segmentEnd) const;

    Extents _extents;
    std::vector<Obstacle*> _obstacles;
    float _agentRadius = 1;
};
