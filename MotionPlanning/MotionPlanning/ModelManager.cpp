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
std::vector<float*> ModelManager::guymodels;
std::vector<Model*> ModelManager::models_;
int ModelManager::num_verts_;
int ModelManager::current_dude;

void ModelManager::InitModels() {
    SphereModel = new Model("models/sphere.txt");
    CubeModel = new Model("models/cube.txt");
    BirdModel = new Model("models/bird.dae");
    DudeModel = new Model("models/dudes/True guy1.obj");
    SeedModel = new Model("models/seed.obj");
    landscape = new LandScape(100, 100, 3);
    //LandscapeModel = new Model("landscape.lan");  // landscape->model;
    LandscapeModel = landscape->model;
    printf("\n\n Land size is: %d\n\n", landscape->model->NumElements());
    printf("\n\n Model size is: %d\n\n", LandscapeModel->model_[0]);

    TreeModel = new Model("models/Tree1.obj");
    guymodels = std::vector<float*>();
    
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy1.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy2.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy3.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy4.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy5.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy6.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy7.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy8.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy9.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy10.obj"));
    guymodels.push_back(Model::LoadObjtoModel("models/dudes/True guy11.obj"));
    current_dude = 0;
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

void ModelManager::updateVBO() {
    int current_offset = 0;
    float* model_data = new float[NumElements()];
    for (auto model : models_) {
        std::copy(model->model_, model->model_ + model->NumElements(), model_data + current_offset);
        current_offset += model->NumElements();
    }
    glBindBuffer(GL_ARRAY_BUFFER,
                 ShaderManager::EnvironmentShader.VBO);  // Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, NumElements() * sizeof(float), model_data, GL_STATIC_DRAW);  // upload vertices to vbo
    delete model_data;
}

void ModelManager::update() {
    if (current_dude > 11) current_dude = 0;

    DudeModel->setModel(guymodels[current_dude]);
    current_dude++;
    updateVBO();
}

void ModelManager::Cleanup() {
    glDeleteBuffers(1, &ShaderManager::EnvironmentShader.VBO);
}

int ModelManager::NumElements() {
    return num_verts_ * ELEMENTS_PER_VERT;
}
