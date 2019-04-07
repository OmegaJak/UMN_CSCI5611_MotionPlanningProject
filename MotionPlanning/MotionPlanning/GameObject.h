#pragma once
#include <detail/type_mat.hpp>
#include "Constants.h"
#include "Material.h"
#include "Model.h"
#include "TextureManager.h"
#include "glm.hpp"

class GameObject {
   public:
    GameObject();
    explicit GameObject(Model* model);
    virtual ~GameObject();

    void SetPosition(const glm::vec3& position);
    void SetScale(float x, float y, float z);
    void SetScale(float n);
    void EulerRotate(float yawDeg, float pitchDeg, float rollDeg);
    void LookAt(const glm::vec3& position, const glm::vec3& up = UP);
    void SetColor(const glm::vec3& color);
    void SetTextureIndex(TEXTURE texture_index);

    float DistanceFrom(GameObject* other) const;

    virtual void Update();

    Material _material;

   protected:
    void CalculateTransform();

    Model* _model;
    TEXTURE _texture_index;
    glm::mat4 _transform;
    glm::mat4 _scale;
    glm::mat4 _rotation;
    glm::vec3 _position;
};
