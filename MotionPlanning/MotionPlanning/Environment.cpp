#include "ClothManager.h"
#include "Environment.h"
#include "Utils.h"

Environment::Environment() {
    _cubeModel = new Model("models/cube.txt");
    _sphereModel = new Model("models/sphere.txt");

    CreateEnvironment();
}

void Environment::UpdateAll() {
    for (auto gameObject : _gameObjects) {
        gameObject->Update();
    }
}

void Environment::SetGravityCenterPosition(const glm::vec3& position) {
    _gameObjects[_gravityCenterIndex]->SetPosition(position);
}

void Environment::CreateEnvironment() {
    GameObject* gameObject;

    gameObject = new GameObject(_cubeModel);  // ground
    gameObject->SetTextureIndex(UNTEXTURED);
    gameObject->SetColor(glm::vec3(0, 77 / 255.0, 26 / 255.0));
    gameObject->SetScale(20, 20, 1);
    gameObject->SetPosition(glm::vec3(0, 0, -0.5));
    gameObject->material_.specFactor_ = 0.2;
    _gameObjects.push_back(gameObject);

    gameObject = new GameObject(_cubeModel);  // reference person
    gameObject->SetTextureIndex(TEX1);
    gameObject->SetScale(1, 0, -3);
    gameObject->SetPosition(glm::vec3(-10, -10, 0));
    _gameObjects.push_back(gameObject);

    gameObject = new GameObject(_sphereModel);  // Obstacle
    gameObject->SetTextureIndex(UNTEXTURED);
    gameObject->SetColor(glm::vec3(.5f, .2f, .3f));
    gameObject->SetPosition(glm::vec3(0, 0, 0));
    gameObject->SetScale(4, 4, 4);
    _gameObjects.push_back(gameObject);

    gameObject = new GameObject(_sphereModel);  // Start
    gameObject->SetTextureIndex(UNTEXTURED);
    gameObject->SetColor(glm::vec3(.5f, .2f, 1.f));
    gameObject->SetPosition(glm::vec3(-10, -10, 0.5));
    gameObject->SetScale(1, 1, 1);
    _gameObjects.push_back(gameObject);

    gameObject = new GameObject(_sphereModel);  // End
    gameObject->SetTextureIndex(UNTEXTURED);
    gameObject->SetColor(glm::vec3(.5f, 1.f, .3f));
    gameObject->SetPosition(glm::vec3(10, 10, 0.5));
    gameObject->SetScale(1, 1, 1);
    _gameObjects.push_back(gameObject);

    _gravityCenterIndex = _gameObjects.size() - 1;
}
