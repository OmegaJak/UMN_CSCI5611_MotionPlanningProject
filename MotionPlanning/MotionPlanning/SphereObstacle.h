#pragma once
#include "Obstacle.h"

class SphereObstacle : public Obstacle {
   public:
    explicit SphereObstacle(const glm::vec3& position, float radius);

    void InflateToCSpaceObstacleForSphericalAgent(float agentRadius) override;
    bool IsPointInside(glm::vec3 point) const override;
    bool DoesSegmentIntersect(glm::vec3 segmentStart, glm::vec3 segmentEnd) const override;

   private:
    static bool SegmentSphereIntersect(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& sphere_pos, const float sphere_radius);

    glm::vec3 _center;

    float _worldRadius;
    float _cSpaceRadius;
};
