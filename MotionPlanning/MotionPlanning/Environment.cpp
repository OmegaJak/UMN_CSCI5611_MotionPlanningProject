#include "Environment.h"
#include "ModelManager.h"
#include "Utils.h"

Environment::Environment() {
    CreateEnvironment();
}

void Environment::UpdateAll() {
    for (auto gameObject : _gameObjects) {
        gameObject->Update();
    }
    for (auto animatedObject : _animatedObjects) {
        animatedObject->Update();
    }
}
void Environment::SetGravityCenterPosition(const glm::vec3& position) {
    _gameObjects[_gravityCenterIndex]->SetPosition(position);
}

void Environment::CreateEnvironment() {
    GameObject* gameObject;
    AnimatedObject* animatedObject;

    /*gameObject = new GameObject(ModelManager::CubeModel);  // ground
    gameObject->SetTextureIndex(UNTEXTURED);
    gameObject->SetColor(glm::vec3(0, 77 / 255.0, 26 / 255.0));
    gameObject->SetScale(20, 20, 1);
    gameObject->SetPosition(glm::vec3(0, 0, -10.5));
    gameObject->_material.specFactor_ = 0.2;
    _gameObjects.push_back(gameObject);
        */
    gameObject = new GameObject(ModelManager::LandscapeModel);  // ground
    gameObject->SetTextureIndex(TEX0);
    gameObject->SetScale(1, 1, 1);
    gameObject->SetPosition(glm::vec3(-100, -100, -12.5));
    gameObject->_material.specFactor_ = 0.01;
    _gameObjects.push_back(gameObject);

    // gameObject = new GameObject(ModelManager::SphereModel);  // Start
    // gameObject->SetTextureIndex(UNTEXTURED);
    // gameObject->SetColor(glm::vec3(.5f, .2f, 1.f));
    // gameObject->SetPosition(glm::vec3(10, -10, 10));
    // gameObject->SetScale(1, 1, 1);
    //_gameObjects.push_back(gameObject);

    // gameObject = new GameObject(ModelManager::SphereModel);  // End
    // gameObject->SetTextureIndex(UNTEXTURED);
    // gameObject->SetColor(glm::vec3(.5f, 1.f, .3f));
    // gameObject->SetPosition(glm::vec3(-10, 10, -10));
    // gameObject->SetScale(1, 1, 1);
    //_gameObjects.push_back(gameObject);

    /*gameObject = new GameObject(ModelManager::ChildModel);
    gameObject->SetTextureIndex(TEX0);
    gameObject->SetScale(5, 5, 5);
    _gameObjects.push_back(gameObject);

    animatedObject = new AnimatedObject(ModelManager::SphereModel);
    animatedObject->SetTextureIndex(UNTEXTURED);
    animatedObject->SetColor(glm::vec3(.5f, 1.f, .5f));
    animatedObject->SetPosition(glm::vec3(20, 10, 10));
    animatedObject->SetScale(5, 5, 5);
    _animatedObjects.push_back(animatedObject);

        gameObject = new GameObject(ModelManager::SeedModel);
    gameObject->SetTextureIndex(TEX1);
    gameObject->SetScale(1, 1, 1);
    gameObject->SetPosition(glm::vec3(5, 5, 5));
    _gameObjects.push_back(gameObject);*/

    _gravityCenterIndex = _gameObjects.size() - 1;
}
