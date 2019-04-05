#pragma once
#include <cstdlib>
#include <gtx/string_cast.hpp>
#include <iostream>
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
        return ((RandomVector01() * 2.0f) - 1.0f);
    }

    // Returns a random vector whose coordinates all between 0 and 1
    static glm::vec3 RandomVector01() {
        return glm::vec3(Random(), Random(), Random());
    }

    static void PrintPoints(const std::vector<glm::vec3>& points) {
        std::cout << "[";
        for (const glm::vec3& point : points) {
            std::cout << glm::to_string(point) << ", ";
        }

        std::cout << "]" << std::endl;
    }

    static void PrintPoints(const std::vector<glm::vec2>& points) {
        std::cout << "[";
        for (const glm::vec2& point : points) {
            std::cout << glm::to_string(point) << ", ";
        }

        std::cout << "]" << std::endl;
    }
};
