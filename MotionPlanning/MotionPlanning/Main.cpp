// ParticleSystem, Jackson Kruger, 2019
// Based on MazeGame, Jackson Kruger, 2018
// Credit to Stephen J. Guy, 2018 for the foundations

#define GLM_FORCE_RADIANS
#include <SDL_image.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "Agent.h"
#include "AgentManager.h"
#include "AnimatedObject.h"
#include "BoxObstacle.h"
#include "Camera.h"
#include "Constants.h"
#include "DebugManager.h"
#include "Environment.h"
#include "GameObject.h"
#include "MotionPlanner.h"
#include "ShaderManager.h"
#include "Skybox.h"
#include "SphereObstacle.h"
#include "TextureManager.h"
#include "Utils.h"
const char* INSTRUCTIONS =
    "***************\n"
    "INSTRUCTIONS\n"
    "***************\n";

#include "glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <cstdio>
#include <string>

#include "ModelManager.h"

using namespace std;

int screenWidth = 1536;
int screenHeight = 864;
float timePassed = 0;
bool fullscreen = false;

std::ostream& operator<<(std::ostream& out, glm::vec3 const& vec) {
    out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return out;
}

void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                              const void* userParam);
float* GetParameterToTweak(SDL_Keycode keycode);

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);  // Initialize Graphics (for OpenGL)

    // Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Create a window (offsetx, offsety, width, height, flags)
    SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

    // Maximize the window if no size was specified
    SDL_SetWindowResizable(window, SDL_TRUE);                // Allow resizing
    SDL_MaximizeWindow(window);                              // Maximize
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);  // Get the new size
    SDL_SetWindowResizable(window, SDL_FALSE);               // Disable future resizing

    // Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_SetRelativeMouseMode(SDL_TRUE);  // 'grab' the mouse

    // Load OpenGL extentions with GLAD
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    } else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }

    // OpenGL debug
    // https://learnopengl.com/In-Practice/Debugging
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    SDL_GL_SetSwapInterval(1);

    srand(time(NULL));

    Camera camera = Camera();

    ModelManager::InitModels();

    Extents envExtents = Extents(glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10));
    ConfigurationSpace cSpace = ConfigurationSpace(envExtents);
    Environment environment = Environment(&cSpace);

    // Cup
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-7, -7, -7), glm::vec3(7, -6, 7)));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-7, -6, -7), glm::vec3(-6, 3, 7)));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(6, -6, -7), glm::vec3(7, 3, 7)));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-6, -6, 6), glm::vec3(6, 3, 7)));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-6, -6, -7), glm::vec3(6, 3, -6)));

    // Cup lid where PRM isn't dense enough
    // cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-6, 2, -4), glm::vec3(6, 3, 6)));

    // Maze
    /*auto xLen = 0.5f;
    auto yLen = 0.5f;
    auto zLen = 20.0f;
    for (int i = 0; i < 8; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-7, 9 - i * 2, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 7; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-5 + i * 2, -5, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 6; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(7, -3 + i * 2, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 5; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(5 - i * 2, 7, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 4; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-3, 5 - i * 2, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 3; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-1 + i * 2, -1, 0), xLen, yLen, zLen));
    }
    for (int i = 0; i < 2; i++) {
        cSpace.AddObstacle(new AABoxObstacle(glm::vec3(3, 1 + i * 2, 0), xLen, yLen, zLen));
    }
    cSpace.AddObstacle(new AABoxObstacle(glm::vec3(1, 3, 0), xLen, yLen, zLen));*/

    // Cross
    cSpace.AddObstacle(new AABoxObstacle(glm::vec3(0, 0, 0), 2, 2, 2));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(6.25, 0, 0), 7.5, 5, 10));
    //cSpace.AddObstacle(new AABoxObstacle(glm::vec3(-6.25, 0, 0), 7.5, 5, 10));

    // cSpace.AddObstacle(new SphereObstacle(glm::vec3(0, 0, 10), 4));
    // cSpace.AddObstacle(new SphereObstacle(glm::vec3(8, -2, 5), 2));

    MotionPlanner motionPlanner = MotionPlanner(cSpace);
    AgentManager agentManager = AgentManager(&motionPlanner, &environment);

    vector<std::string> faces = {"images/right2.png", "images/left2.png", "images/bottom2.png",
                                 "images/top2.png",   "images/back2.png", "images/front2.png"};
    Skybox skybox = Skybox(faces);

    // Render the axes
    LineIndexRange lineIndices = DebugManager::RequestLines(3);
    DebugManager::SetLine(lineIndices.firstIndex, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1));
    DebugManager::SetLine(1, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0));
    DebugManager::SetLine(lineIndices.lastIndex, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    ShaderManager::InitShaders();
    TextureManager::InitTextures();

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_CULL_FACE);

    printf("%s\n", INSTRUCTIONS);

    // Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    bool quit = false;
    float lastTickTime = 0;
    int frameCounter = 0;
    float lastFramesTimer = 0;
    float lastFramerate = 0;
    int framesPerSample = 20;
    float lastAverageFrameTime = 0;
    float* lastSelectedParam = nullptr;
    float modAmount = 0.1;
    while (!quit) {
        while (SDL_PollEvent(&windowEvent)) {  // inspect all events in the queue
            if (windowEvent.type == SDL_QUIT) quit = true;
            // List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
            // Scancode refers to a keyboard position, keycode refers to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_KEYUP) {  // Exit event loop
                if (windowEvent.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                } else if (windowEvent.key.keysym.sym == SDLK_F11) {  // If F11 is pressed
                    fullscreen = !fullscreen;
                    SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);  // Toggle fullscreen
                } else if (windowEvent.key.keysym.sym == SDLK_EQUALS || windowEvent.key.keysym.sym == SDLK_MINUS) {
                    if (windowEvent.key.keysym.mod & KMOD_ALT) {
                        if (windowEvent.key.keysym.sym == SDLK_EQUALS) {
                            modAmount *= 10;
                        } else if (windowEvent.key.keysym.sym == SDLK_MINUS) {
                            modAmount *= 0.1;
                        }
                    } else {
                        float tempMod = modAmount;
                        if (windowEvent.key.keysym.sym == SDLK_MINUS) tempMod *= -1;
                        if (windowEvent.key.keysym.mod & KMOD_SHIFT) tempMod *= 10;
                        if (windowEvent.key.keysym.mod & KMOD_CTRL) tempMod *= 0.1;

                        if (lastSelectedParam != nullptr) *lastSelectedParam += tempMod;
                    }
                } else if (windowEvent.key.keysym.sym == SDLK_r) {
                    agentManager.Reset();
                } else if (windowEvent.key.keysym.sym == SDLK_l) {
                    DebugManager::ShouldRenderDebugLines = !DebugManager::ShouldRenderDebugLines;
                } else if (windowEvent.key.keysym.sym == SDLK_f) {
                    environment.addSeed(camera.GetPosition());
                }

                if (GetParameterToTweak(windowEvent.key.keysym.sym) != nullptr)
                    lastSelectedParam = GetParameterToTweak(windowEvent.key.keysym.sym);
            } else if (windowEvent.type == SDL_KEYDOWN) {
                if (windowEvent.key.keysym.sym == SDLK_SPACE) {
                    AgentManager::AgentsRunning = !AgentManager::AgentsRunning;
                }
            }

            if (windowEvent.type == SDL_MOUSEMOTION && SDL_GetRelativeMouseMode() == SDL_TRUE) {
                camera.ProcessMouseInput(windowEvent.motion.xrel, windowEvent.motion.yrel);
            }

            switch (windowEvent.window.event) {
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    SDL_Log("Window focus lost");
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    SDL_Log("Window focus gained");
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                default:
                    break;
            }
        }

        auto time = SDL_GetTicks() / 1000.0f;
        float deltaTime = time - lastTickTime;
        lastTickTime = time;

        frameCounter++;
        lastFramesTimer += deltaTime;
        if (frameCounter >= framesPerSample) {
            lastAverageFrameTime = (lastFramesTimer * 1000.0f) / framesPerSample;
            lastFramerate = 1.0f / (lastFramesTimer / framesPerSample);

            frameCounter = 0;
            lastFramesTimer = 0;
        }

        // Rendering //
        float gray = 0.6f;
        glClearColor(gray, gray, gray, 1.0f);  // Clear the screen to default color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(3.14f / 2, screenWidth / (float)screenHeight, 0.1f, 1000.0f);  // FOV, aspect, near, far
        ShaderManager::ApplyToEachRenderShader(
            [proj](ShaderAttributes attributes) -> void { glUniformMatrix4fv(attributes.projection, 1, GL_FALSE, glm::value_ptr(proj)); },
            PROJ_SHADER_FUNCTION_ID);

        stringstream debugText;
        debugText << fixed << setprecision(4) << lastAverageFrameTime << "ms per frame (" << lastFramerate << "FPS) average over "
                  << framesPerSample
                  << " frames "
                  //<< " | cameraPosition: " << camera.GetPosition();
                  << " | mod: " << modAmount        //
                  << " | damp: " << Agent::Damping  //
                  << " | Separation: " << Agent::SeparationParams.radius << ", " << Agent::SeparationParams.strength
                  << " | Cohesion: " << Agent::CohesionParams.radius << ", " << Agent::CohesionParams.strength
                  << " | Alignment: " << Agent::AlignmentParams.radius << ", " << Agent::AlignmentParams.strength
                  << " | Obstacle: " << Agent::ObstacleParams.radius << ", " << Agent::ObstacleParams.strength
                  << " | Path: " << Agent::PathParams.radius << ", " << Agent::PathParams.strength;
        SDL_SetWindowTitle(window, debugText.str().c_str());

        // Render the environment
        skybox.Update(camera.GetView(), proj);
        ShaderManager::ActivateShader(ShaderManager::EnvironmentShader);
        glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::EnvironmentShader.VBO);
        TextureManager::Update(ShaderManager::EnvironmentShader.Program);

        // environment.updateDude(camera.GetPosition());

        environment.UpdateAll();
        environment.ProcessKeyboardInput(camera);
        camera.setPosition(environment.getObject()->getPosition());

        camera.Update();

        agentManager.Update();
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render the PBR (DEBUG ONLY)
        motionPlanner.Update();

        // Render Debug Lines
        ShaderManager::ActivateShader(ShaderManager::DebugShader);
        glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::DebugShader.VBO);
        DebugManager::Draw();
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        SDL_GL_SwapWindow(window);  // Double buffering
    }

    // Clean Up
    ShaderManager::Cleanup();
    ModelManager::Cleanup();

    SDL_GL_DeleteContext(context);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

// https://learnopengl.com/In-Practice/Debugging
void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                              const void* userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

float* GetParameterToTweak(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_1:
            return &Agent::SeparationParams.radius;
        case SDLK_2:
            return &Agent::SeparationParams.strength;
        case SDLK_3:
            return &Agent::CohesionParams.radius;
        case SDLK_4:
            return &Agent::CohesionParams.strength;
        case SDLK_5:
            return &Agent::AlignmentParams.radius;
        case SDLK_6:
            return &Agent::AlignmentParams.strength;
        case SDLK_7:
            return &Agent::ObstacleParams.radius;
        case SDLK_8:
            return &Agent::ObstacleParams.strength;
        case SDLK_9:
            return &Agent::PathParams.radius;
        case SDLK_0:
            return &Agent::PathParams.strength;
        case SDLK_BACKQUOTE:
            return &Agent::Damping;
        default:
            return nullptr;
    }
}
