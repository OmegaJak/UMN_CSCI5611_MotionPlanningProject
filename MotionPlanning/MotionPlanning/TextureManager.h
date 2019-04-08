#pragma once
#include "SDL.h"
#include "glad.h"

typedef enum { UNTEXTURED = -1, TEX0 = 0, TEX1 = 1, TEX2 = 2, TEX3 = 3, TEX4 = 4, TEX5 = 5 } TEXTURE;

class TextureManager {
   public:
    static void InitTextures();
    static void Update(GLuint program);

   private:
    static void InitTexture(GLuint* tex_location, SDL_Surface* surface);

    static GLuint tex0;
    static GLuint tex1;
    static GLuint tex2;
    static GLuint tex3;
    static GLuint tex4;
    static GLuint tex5;
};
