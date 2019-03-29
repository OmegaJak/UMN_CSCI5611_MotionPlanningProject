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

   private:
    static std::vector<Model*> models_;
    static int num_verts_;
};
