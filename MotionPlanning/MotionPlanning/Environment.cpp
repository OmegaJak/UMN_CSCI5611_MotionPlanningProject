#include "Environment.h"
#include "ModelManager.h"
#include "Utils.h"
#include "Camera.h"

Environment::Environment() {
    CreateEnvironment();
}

void Environment::UpdateAll() {
    main_character->Update();
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

	main_character = new GameObject(ModelManager::DudeModel);
    main_character->SetTextureIndex(TEX4);
    main_character->SetScale(.10, .10, .10);
    main_character->SetPosition(glm::vec3(13, -12, 5.0));
    main_character->_material.specFactor_ = 0.3;

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

void Environment::updateDude(glm::vec3 pos) {
    main_character->SetPosition(pos - glm::vec3(0, 0, 1));
}

void Environment::ProcessKeyboardInput(Camera c) {
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    auto rotateSpeed = CAMERA_ROTATION_SPEED;
    auto moveSpeed = CAMERA_MOVE_SPEED;
    if (key_state[SDL_SCANCODE_LSHIFT]) {
        moveSpeed = MAX_MOVE_SPEED;
    }

    // Look up/down
    if (key_state[SDL_SCANCODE_UP]) {
        //_pitch += rotateSpeed;
    } else if (key_state[SDL_SCANCODE_DOWN]) {
        //_pitch -= rotateSpeed;
    }

    // Look right/left
    if (key_state[SDL_SCANCODE_RIGHT]) {
        //_yaw -= rotateSpeed;
    } else if (key_state[SDL_SCANCODE_LEFT]) {
        //_yaw += rotateSpeed;
    }
    glm::vec3 pos = main_character->getPosition();
    glm::vec3 _forward = c.getForward();
    glm::vec3 _up = c.getUp();
    glm::vec3 _right = c.getRight();

    // Forward/back
    if (key_state[SDL_SCANCODE_W]) {
        pos += _forward * moveSpeed;
    } else if (key_state[SDL_SCANCODE_S]) {
        pos -= _forward * moveSpeed;
    }

    // Right/left
    if (key_state[SDL_SCANCODE_D]) {
        pos += _right * moveSpeed;
    } else if (key_state[SDL_SCANCODE_A]) {
        pos -= _right * moveSpeed;
    }

    // Up/down
    if (key_state[SDL_SCANCODE_E]) {
        pos += _up * moveSpeed;
    } else if (key_state[SDL_SCANCODE_Q]) {
        pos -= _up * moveSpeed;
    }

	main_character->SetPosition(pos);
}