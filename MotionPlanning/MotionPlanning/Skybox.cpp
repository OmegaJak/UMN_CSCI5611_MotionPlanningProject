#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include "Constants.h"
#include "Skybox.h"
#include "ShaderManager.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"


void Skybox::InitVBO() {
    glGenTextures(1, &CubeID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeID);

	//Initialize the six images
	IMG_Init(IMG_INIT_PNG);
    // Allocate Texture 0
    for (unsigned int i = 0; i < faces_.size(); i++) {
        SDL_Surface* surface = IMG_Load(faces_[i].c_str());
        if (surface->pixels) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
            printf("Height: %d, Width: %d \n", surface->w, surface->h);
        } else {
            printf("Cubemap texture failed to load");
            exit(1);
        }
    }

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Initialize the Shader
    skyShader = InitShader("Sky_Vertex.glsl", "Sky_Fragment.glsl");

	// Initialize the Vao, and Vbo
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    suniView = glGetUniformLocation(skyShader, "view");
    suniProj = glGetUniformLocation(skyShader, "projection");
	}
	
void Skybox::Update(glm::mat4 view, glm::mat4 proj) {
        glDepthMask(GL_FALSE);
        // Draw Skybox //
        glUseProgram(skyShader);
        glm::mat4 newview = glm::mat4(glm::mat3(view));
        newview = glm::rotate(newview, (float)M_PI / 2, glm::vec3(1.f, 0.0f, 0.0f));
        newview = glm::rotate(newview, (float)M_PI / 4, glm::vec3(0.f, 1.0f, 0.0f));
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubeID);
        glUniform1i(glGetUniformLocation(skyShader, "skybox"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glUniformMatrix4fv(suniView, 1, GL_FALSE, glm::value_ptr(newview));
        glUniformMatrix4fv(suniProj, 1, GL_FALSE, glm::value_ptr(proj));
        glDepthMask(GL_TRUE);
        // End Draw Skybox //
    }

// Create a GLSL program object from vertex and fragment shader files
    GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName) {
        GLuint vertex_shader, fragment_shader;
        GLchar *vs_text, *fs_text;
        GLuint program;

        // check GLSL version
        printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Create shader handlers
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Read source code from shader files
        vs_text = readShaderSource(vShaderFileName);
        fs_text = readShaderSource(fShaderFileName);

        // error check
        if (vs_text == NULL) {
            printf("Failed to read from vertex shader file %s\n", vShaderFileName);
            exit(1);
        } else if (DEBUG_ON) {
            printf("Vertex Shader:\n=====================\n");
            printf("%s\n", vs_text);
            printf("=====================\n\n");
        }
        if (fs_text == NULL) {
            printf("Failed to read from fragent shader file %s\n", fShaderFileName);
            exit(1);
        } else if (DEBUG_ON) {
            printf("\nFragment Shader:\n=====================\n");
            printf("%s\n", fs_text);
            printf("=====================\n\n");
        }

        // Load Vertex Shader
        const char* vv = vs_text;
        glShaderSource(vertex_shader, 1, &vv, NULL);  // Read source
        glCompileShader(vertex_shader);               // Compile shaders

        // Check for errors
        GLint compiled;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            printf("Vertex shader failed to compile:\n");
            if (DEBUG_ON) {
                GLint logMaxSize, logLength;
                glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
                printf("printing error message of %d bytes\n", logMaxSize);
                char* logMsg = new char[logMaxSize];
                glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
                printf("%d bytes retrieved\n", logLength);
                printf("error message: %s\n", logMsg);
                delete[] logMsg;
            }
            Sleep(10000);
            exit(1);
        }

        // Load Fragment Shader
        const char* ff = fs_text;
        glShaderSource(fragment_shader, 1, &ff, NULL);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

        // Check for Errors
        if (!compiled) {
            printf("Fragment shader failed to compile\n");
            if (DEBUG_ON) {
                GLint logMaxSize, logLength;
                glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
                printf("printing error message of %d bytes\n", logMaxSize);
                char* logMsg = new char[logMaxSize];
                glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
                printf("%d bytes retrieved\n", logLength);
                printf("error message: %s\n", logMsg);
                delete[] logMsg;
            }
            Sleep(10000);
            exit(1);
        }

        // Create the program
        program = glCreateProgram();

        // Attach shaders to program
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // Link and set program to use
        glLinkProgram(program);
        return program;
    }

	// Create a NULL-terminated string by reading the provided file
    static char* readShaderSource(const char* shaderFile) {
        FILE* fp;
        long length;
        char* buffer;

        // open the file containing the text of the shader code
        fp = fopen(shaderFile, "r");

        // check for errors in opening the file
        if (fp == NULL) {
            printf("can't open shader source file %s\n", shaderFile);
            return NULL;
        }

        // determine the file size
        fseek(fp, 0, SEEK_END);  // move position indicator to the end of the file;
        length = ftell(fp);      // return the value of the current position

        // allocate a buffer with the indicated number of bytes, plus one
        buffer = new char[length + 1];

        // read the appropriate number of bytes from the file
        fseek(fp, 0, SEEK_SET);        // move position indicator to the start of the file
        fread(buffer, 1, length, fp);  // read all of the bytes

        // append a NULL character to indicate the end of the string
        buffer[length] = '\0';

        // close the file
        fclose(fp);

        // return the string
        return buffer;
    }