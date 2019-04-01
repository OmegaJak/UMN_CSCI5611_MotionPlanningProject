#pragma once
#include <detail/type_mat.hpp>
#include "Material.h"
#include "Model.h"
#include "TextureManager.h"
#include "glm.hpp"
#include "GameObject.h"

class AnimatedObject: public GameObject {
   public:
    AnimatedObject();
    AnimatedObject(Model* model);
};
