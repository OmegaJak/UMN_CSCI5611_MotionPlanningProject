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

        return glm::vec3(x, y, z);
    }
};
