#include <algorithm>
#include "Constants.h"
#include "ModelManager.h"
#include "ShaderManager.h"

Model* ModelManager::SphereModel;
Model* ModelManager::CubeModel;
Model* ModelManager::BirdModel;
Model* ModelManager::DudeModel;
Model* ModelManager::SeedModel;
Model* ModelManager::TreeModel;
LandScape* ModelManager::landscape;
Model* ModelManager::LandscapeModel;
std::vector<Model*> ModelManager::models_;
int ModelManager::num_verts_;

void ModelManager::InitModels() {
    SphereModel = new Model("models/sphere.txt");
    CubeModel = new Model("models/cube.txt");
    BirdModel = new Model("models/bird.dae");
    DudeModel = new Model("models/Dude.obj");
    SeedModel = new Model("models/seed.obj");
    landscape = new LandScape(100, 100, 3);
    //LandscapeModel = new Model("landscape.lan");  // landscape->model;
    LandscapeModel = landscape->model;
    printf("\n\n Land size is: %d\n\n", landscape->model->NumElements());
    printf("\n\n Model size is: %d\n\n", LandscapeModel->model_[0]);

    TreeModel = new Model("models/Tree1.obj");
}

void ModelManager::RegisterModel(Model* model) {
    models_.push_back(model);
    model->vbo_vertex_start_index_ = num_verts_;
    num_verts_ += model->NumVerts();
}

void ModelManager::InitVBO() {
    float* model_data = new float[NumElements()];

    int current_offset = 0;

    for (auto model : models_) {
        printf("Size of Model: %d", model->NumElements());
        std::copy(model->model_, model->model_ + model->NumElements(), model_data + current_offset);
        current_offset += model->NumElements();
    }

    glGenBuffers(1, &ShaderManager::EnvironmentShader.VBO);  // Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER,
                 ShaderManager::EnvironmentShader.VBO);  // Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, NumElements() * sizeof(float), model_data, GL_STATIC_DRAW);  // upload vertices to vbo
}

void ModelManager::Cleanup() {
    glDeleteBuffers(1, &ShaderManager::EnvironmentShader.VBO);
}

int ModelManager::NumElements() {
    return num_verts_ * ELEMENTS_PER_VERT;
}
