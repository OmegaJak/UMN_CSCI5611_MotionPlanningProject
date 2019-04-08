#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include "ShaderManager.h"
#include "TextureManager.h"
#include "glad.h"

GLuint TextureManager::tex0;
GLuint TextureManager::tex1;
GLuint TextureManager::tex2;
GLuint TextureManager::tex3;
GLuint TextureManager::tex4;
GLuint TextureManager::tex5;

void TextureManager::InitTextures() {
    IMG_Init(IMG_INIT_PNG);

    glBindVertexArray(ShaderManager::EnvironmentShader.VAO);

    // Allocate Texture 0
    SDL_Surface* surface = IMG_Load("images/bird01.png");
    glActiveTexture(GL_TEXTURE0);
    InitTexture(&tex0, surface);

    glActiveTexture(GL_TEXTURE1);
    surface = IMG_Load("images/Seed.png");
    InitTexture(&tex1, surface);

    glActiveTexture(GL_TEXTURE2);
    surface = IMG_Load("images/grass.png");
    InitTexture(&tex2, surface);

	glActiveTexture(GL_TEXTURE3);
    surface = IMG_Load("images/Trees.png");
    InitTexture(&tex3, surface);

	glActiveTexture(GL_TEXTURE4);
    surface = IMG_Load("images/Dude.png");
    InitTexture(&tex4, surface);

	glActiveTexture(GL_TEXTURE5);
    surface = IMG_Load("images/Trees.png");
    InitTexture(&tex5, surface);


    glBindVertexArray(0);
}

void TextureManager::Update(GLuint program) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(program, "tex0"), TEX0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(program, "tex1"), TEX1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(glGetUniformLocation(program, "tex2"), TEX2);

	glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex3);
    glUniform1i(glGetUniformLocation(program, "tex3"), TEX3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tex4);
    glUniform1i(glGetUniformLocation(program, "tex4"), TEX4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex5);
    glUniform1i(glGetUniformLocation(program, "tex5"), TEX5);
}

// http://www.sdltutorials.com/sdl-tip-sdl-surface-to-opengl-texture
void TextureManager::InitTexture(GLuint* tex_location, SDL_Surface* surface) {
    if (surface == NULL) {  // If it failed, print the error
        printf("Error: \"%s\"\n", SDL_GetError());
        exit(1);
    }

    glGenTextures(1, tex_location);
    glBindTexture(GL_TEXTURE_2D, *tex_location);

    // What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int mode = GL_RGB;
    if (surface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    }

    // Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);  // Mip maps the texture

    // Distance filtering method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
}
