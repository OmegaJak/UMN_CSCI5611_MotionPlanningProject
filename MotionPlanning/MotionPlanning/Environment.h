#pragma once
#include "AnimatedObject.h"
#include "Camera.h"
#include "GameObject.h"


typedef struct {
    glm::vec3 position;
    glm::vec3 velocity;
} Seed;

class Environment {
   public:
    Environment();

    void addSeed(glm::vec3 pos);
    void updateDude(glm::vec3 pos);
    void ProcessKeyboardInput(Camera c);
    void UpdateAll();
    void SetGravityCenterPosition(const glm::vec3& position);
    GameObject* getObject() {
        return main_character;
    }

   private:
    void CreateEnvironment();
    GameObject* main_character;
    std::vector<GameObject*> _seeds;
    std::vector<Seed*> _seedattribs;
    std::vector<GameObject*> _gameObjects;
    std::vector<AnimatedObject*> _animatedObjects;
    int _gravityCenterIndex;
};
