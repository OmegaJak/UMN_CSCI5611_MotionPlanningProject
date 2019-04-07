#pragma once
#include "GameObject.h"

class Obstacle {
   public:
    virtual void InflateToCSpaceObstacleForSphericalAgent(float agentRadius) = 0;
    virtual bool IsPointInside(glm::vec3 point) const = 0;
    virtual bool DoesSegmentIntersect(glm::vec3 segmentStart, glm::vec3 segmentEnd) const = 0;
    virtual float GetDistanceFromPointToSurface(const glm::vec3& point) const = 0;
    virtual glm::vec3 GetRepulsionNormalToPoint(const glm::vec3& point) const = 0;

    void Update();

   protected:
    GameObject _environmentObstacle;
};
