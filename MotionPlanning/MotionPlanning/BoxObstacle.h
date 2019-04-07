#pragma once
#include "Obstacle.h"

class AABoxObstacle : public Obstacle {
   public:
    AABoxObstacle(const glm::vec3& center, float xLength, float yLength, float zLength);
    AABoxObstacle(const glm::vec3& min, const glm::vec3& max);

    void InflateToCSpaceObstacleForSphericalAgent(float agentRadius) override;
    bool IsPointInside(glm::vec3 point) const override;
    bool DoesSegmentIntersect(glm::vec3 segmentStart, glm::vec3 segmentEnd) const override;
    float GetDistanceFromPointToSurface(const glm::vec3& point) const override;
    glm::vec3 GetRepulsionNormalToPoint(const glm::vec3& point) const override;

   private:
    void InitRendering();

    glm::vec3 _min, _max, _center, _halfSideLength;
};
