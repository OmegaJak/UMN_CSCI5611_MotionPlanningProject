#pragma once
#include "GameObject.h"
#include "AnimatedObject.h"

class Environment {
   public:
    Environment();

    void UpdateAll();
    void SetGravityCenterPosition(const glm::vec3& position);

    GameObject* getObject() {
        return _gameObjects[1];
    }

   private:
    void CreateEnvironment();

    std::vector<GameObject*> _gameObjects;
    std::vector<AnimatedObject*> _animatedObjects;
    int _gravityCenterIndex;
};
