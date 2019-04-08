#include "Camera.h"
#include "Environment.h"
#include "ModelManager.h"
#include "Utils.h"

Environment::Environment(ConfigurationSpace* cSpace) {
    CreateEnvironment();
    _cSpace = cSpace;
}

void Environment::UpdateAll() {
    main_character->Update();
    glm::vec3 pos = main_character->getPosition();
    pos.z = ModelManager::landscape->get_height_at(glm::vec3((pos.x) / 3 + 33.3333, (pos.y) / 3 + 33.333, pos.z)) * 3.f - 4.3f;
    main_character->SetPosition(pos - glm::vec3(0, 0, 11));

    for (auto gameObject : _gameObjects) {
        gameObject->Update();
    }
    for (auto animatedObject : _animatedObjects) {
        animatedObject->Update();
    }

    std::vector<Seed*> seedsToRemove = {};
    for (auto& seedattrib : _seedattribs) {
        GameObject* seedGameObject = seedattrib->gameObject;
        glm::vec3 seedpos = seedGameObject->getPosition();
        float h = ModelManager::landscape->get_height_at(glm::vec3((seedpos.x) / 3 + 33.3333, (seedpos.y) / 3 + 33.333, seedpos.z)) * 3.f - 15.f;
        if (seedpos.z < h) {
            seedpos.z = h - 0.3f;
            seedGameObject->SetPosition(seedpos);
            seedattrib->velocity = glm::vec3(0, 0, 0);
            if (_cSpace->PointIsInsideObstacle(seedattrib->position)) {
                seedsToRemove.push_back(seedattrib);
            }
        } else {
            seedpos += seedattrib->velocity;
            seedattrib->velocity -= glm::vec3(0, 0, .01f);
        }
        if (seedpos != seedGameObject->getPosition()) {
            seedGameObject->SetPosition(seedpos);
            seedattrib->position = seedpos;
        }

        seedGameObject->Update();
    }

    for (Seed* seedToRemove : seedsToRemove) {
        RemoveSeed(seedToRemove);
    }
}
void Environment::SetGravityCenterPosition(const glm::vec3& position) {
    _gameObjects[_gravityCenterIndex]->SetPosition(position);
}

Seed* Environment::GetClosestSeedTo(const glm::vec3& position) {
    float minDist = INFINITY;
    Seed* closestSeed = nullptr;
    for (auto seed : _seedattribs) {
        auto dist = glm::distance(position, seed->position);
        if (dist < minDist && seed->velocity == glm::vec3(0, 0, 0)) {
            minDist = dist;
            closestSeed = seed;
        }
    }

    return closestSeed;
}

void Environment::RemoveSeed(Seed* seed) {
    auto index = std::find(_seedattribs.begin(), _seedattribs.end(), seed);
    if (index != _seedattribs.end()) {
        _seedattribs.erase(index);
    }
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
    gameObject->SetPosition(glm::vec3(-100, -100, -15.3));
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
    for (int i = 0; i < 12; i++) {
        seed = new GameObject(ModelManager::SeedModel);
        seed->SetTextureIndex(TEX1);
        seed->SetScale(.06, .06, .06);
        seed->_material.specFactor_ = .3;
        seedatt = new Seed();
        newpos = pos + .1f * Utils::RandomVector();
        seedatt->position = newpos;
        seedatt->velocity = .1f * Utils::RandomVector();
        seedatt->gameObject = seed;
        seed->SetPosition(newpos + glm::vec3(0, 0, 2));

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
