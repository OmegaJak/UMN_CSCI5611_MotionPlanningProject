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

	for (int i = 0; i < _seeds.size(); i++) {
		GameObject* seed = _seeds[i];
		Seed* seedatt = _seedattribs[i];
		glm::vec3 seedpos = seed->getPosition();
		if (seedpos.z <= -10.01) {
			seedpos.z = -10.02;
			seed->SetPosition(seedpos);
		}
		else {
			seedpos += seedatt->velocity;
			seedatt->velocity -= glm::vec3(0, 0, .01f);
		}
		seed->SetPosition(seedpos);

		seed->Update();
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
    gameObject->SetTextureIndex(TEX2);
    gameObject->SetScale(1, 1, 1);
    gameObject->SetPosition(glm::vec3(-100, -100, -13.3));
    gameObject->_material.specFactor_ = 0.01;
    _gameObjects.push_back(gameObject);

	gameObject = new GameObject(ModelManager::TreeModel);  // ground
	gameObject->SetTextureIndex(TEX3);
	gameObject->SetScale(.31, .31, .25);
	gameObject->SetPosition(glm::vec3(0, 10, 4));
	gameObject->_material.specFactor_ = 0.3;
	_gameObjects.push_back(gameObject);

    _gravityCenterIndex = _gameObjects.size() - 1;
}

void Environment::addSeed(glm::vec3 pos) {
	GameObject* seed;
	Seed* seedatt;
	glm::vec3 newpos;
	for (int i = 0; i < 10; i++) {
		seed = new GameObject(ModelManager::SeedModel);
		seed->SetTextureIndex(TEX1);
		seed->SetScale(.2, .2, .2);
		seed->_material.specFactor_ = .3;
		seedatt = new Seed();
		newpos = pos + .1f * Utils::RandomVector();
		seedatt->position = newpos;
		seedatt->velocity = .1f * Utils::RandomVector();
		seed->SetPosition(newpos);

		_seeds.push_back(seed);
		_seedattribs.push_back(seedatt);
	}

}