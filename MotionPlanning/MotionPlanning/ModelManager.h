#pragma once
#include <vector>
#include "Model.h"
#include "glad.h"

class ModelManager {
   public:
    static void InitModels();
    static void RegisterModel(Model* model);

    static void InitVBO();
    static void Cleanup();

    static int NumElements();

    static Model* SphereModel;
    static Model* CubeModel;
    static Model* BirdModel;
    static Model* DudeModel;
    static Model* SeedModel;
    static Model* LandscapeModel;
    static Model* TreeModel;

   private:
    static std::vector<Model*> models_;
    static int num_verts_;
};
