#define GLM_FORCE_RADIANS
#include <SDL_stdinc.h>
#include <gtc/type_ptr.hpp>
#include <gtx/euler_angles.hpp>
#include "Constants.h"
#include "AnimatedObject.h"
#include "ShaderManager.h"
#include "glad.h"

AnimatedObject::AnimatedObject(Model* model) : GameObject(model) {
}
