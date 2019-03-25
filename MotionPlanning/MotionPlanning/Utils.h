#pragma once
#include <cstdlib>
#include "MotionPlanner.h"

class Utils {
   public:
    // https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
    static float randBetween(int min, int max) {
        float random = ((float)rand()) / (float)RAND_MAX;
        float diff = max - min;
        float r = random * diff;
        return min + r;
    }

    static float Random() {
        return rand() / (float)RAND_MAX;
    }

    static float RandomRange(float min, float max) {
        if (min > max) std::swap(min, max);
        return (Random() * (max - min)) + min;
    }

    // Returns a random vector whose coordinates are all between -1 and 1 (inclusive)
    static glm::vec3 RandomVector() {
        float x = (Random() * 2.0f) - 1.0f;
        float y = (Random() * 2.0f) - 1.0f;
        float z = (Random() * 2.0f) - 1.0f;

        return glm::normalize(glm::vec3(x, y, z));
    }

    static bool SegmentSphereIntersect(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& sphere_pos, const float sphere_radius) {
        glm::vec3 d = (p2 - p1);
        float lineLength = glm::length(d);
        d = d / lineLength;
        // Substitute Segment x,y,z into Sphere 3d equation, and solve for t. (Quadratic formula required)

        // if the direction was not normalized, it would look like this: float A = d.x  * d.x + d.y * d.y + d.z * d.z or
        // d.Dot(d); but since it is normalized, A is equal to 1

        // Quadratic formula values:
        glm::vec3 offset =
            p1 - sphere_pos;  // Offset sphere position from origin of segment (v is position of sphere, p is origin of segment)
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
};
