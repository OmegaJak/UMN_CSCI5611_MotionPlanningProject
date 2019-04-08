#pragma once
#include "GameObject.h"
#include "AnimatedObject.h"

typedef struct {
    glm::vec3 position;
    glm::vec3 velocity;
} Seed;

class Environment {
   public:
    Environment();

	void addSeed(glm::vec3 pos);
    void UpdateAll();
    void SetGravityCenterPosition(const glm::vec3& position);

    GameObject* getObject() {
        return _gameObjects[1];
    }

   private:
    void CreateEnvironment();
    std::vector<GameObject*> _seeds;
    std::vector<Seed*> _seedattribs;
    std::vector<GameObject*> _gameObjects;
    std::vector<AnimatedObject*> _animatedObjects;
    int _gravityCenterIndex;
};
