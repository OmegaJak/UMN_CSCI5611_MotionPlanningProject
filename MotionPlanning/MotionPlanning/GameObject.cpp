#define GLM_FORCE_RADIANS
#include <SDL_stdinc.h>
#include <gtc/type_ptr.hpp>
#include <gtx/euler_angles.hpp>
#include "Constants.h"
#include "GameObject.h"
#include "ShaderManager.h"
#include "glad.h"
#include "gtx/rotate_vector.hpp"

GameObject::GameObject() : GameObject(nullptr) {}

GameObject::GameObject(Model* model) : _model(model), _texture_index(UNTEXTURED) {
    _transform = _scale = _rotation = glm::mat4();
    _material = Material(glm::vec3(1, 0, 1));
}

GameObject::~GameObject() = default;

void GameObject::SetTextureIndex(TEXTURE texture_index) {
    _texture_index = texture_index;
}

float GameObject::DistanceFrom(GameObject* other) const {
    return glm::distance(_position, other->_position);
}

void GameObject::Update() {
    if (_model == nullptr) {
        printf("GameObject must be given a valid model before calling Update()\n");
        exit(1);
    }

    glUniformMatrix4fv(ShaderManager::EnvironmentShader.Attributes.model, 1, GL_FALSE,
                       glm::value_ptr(_transform));                                  // pass model matrix to shader
    glUniform1i(ShaderManager::EnvironmentShader.Attributes.texID, _texture_index);  // Set which texture to use
    glUniform1f(ShaderManager::EnvironmentShader.Attributes.specFactor, _material.specFactor_);
    if (_texture_index == UNTEXTURED) {
        glUniform3fv(ShaderManager::EnvironmentShader.Attributes.color, 1,
                     glm::value_ptr(_material.color_));  // Update the color, if necessary
    }

    glDrawArrays(GL_TRIANGLES, _model->vbo_vertex_start_index_, _model->NumVerts());
}

void GameObject::CalculateTransform() {
    _transform = _scale * _rotation;
    _transform[3] = glm::vec4(_position, _scale[3][3]);  // This seems wrong but it fixes things
}

void GameObject::SetPosition(const glm::vec3& position) {
    _position = position;
    CalculateTransform();
}

void GameObject::SetScale(float x, float y, float z) {
    _scale[0][0] = x;
    _scale[1][1] = y;
    _scale[2][2] = z;
    CalculateTransform();
}

void GameObject::SetScale(float n) {
    SetScale(n, n, n);
}

void GameObject::SetScale(glm::vec3 scale) {
    SetScale(scale.x, scale.y, scale.z);
}

void GameObject::EulerRotate(float yawDeg, float pitchDeg, float rollDeg) {
    _rotation = glm::eulerAngleYXZ(glm::radians(yawDeg), glm::radians(pitchDeg), glm::radians(rollDeg));
    CalculateTransform();
}

void GameObject::LookAt(const glm::vec3& position, const glm::vec3& up) {
    _rotation = glm::lookAt(_position, position, up);
    CalculateTransform();
}

void GameObject::SetColor(const glm::vec3& color) {
    _material.color_ = color;
}
