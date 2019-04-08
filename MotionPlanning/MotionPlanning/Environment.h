#pragma once
#include "AnimatedObject.h"
#include "Camera.h"
#include "GameObject.h"

class ConfigurationSpace;

typedef struct {
    glm::vec3 position;
    glm::vec3 velocity;
    GameObject* gameObject;
} Seed;

class Environment {
   public:
    explicit Environment(ConfigurationSpace* cSpace);

    void addSeed(glm::vec3 pos);
    void updateDude(glm::vec3 pos);
    void ProcessKeyboardInput(Camera c);
    void UpdateAll();
    void SetGravityCenterPosition(const glm::vec3& position);
    GameObject* getObject() {
        return main_character;
    }

    Seed* GetClosestSeedTo(const glm::vec3& position);
    void RemoveSeed(Seed* seed);

   private:
    void CreateEnvironment();
    GameObject* main_character;
    std::vector<Seed*> _seedattribs;
    std::vector<GameObject*> _gameObjects;
    std::vector<AnimatedObject*> _animatedObjects;
    int _gravityCenterIndex;
    ConfigurationSpace* _cSpace;
};
