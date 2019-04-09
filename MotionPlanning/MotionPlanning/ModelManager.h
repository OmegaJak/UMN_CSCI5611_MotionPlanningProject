#pragma once
#include <vector>
#include "Landscape.h"
#include "Model.h"
#include "glad.h"

class ModelManager {
   public:
    static void InitModels();
    static void RegisterModel(Model* model);

    static void InitVBO();
    static void updateVBO();
    static void Cleanup();
    static void update();

    static int NumElements();

    static Model* SphereModel;
    static Model* CubeModel;
    static Model* BirdModel;
    static Model* DudeModel;
    static Model* SeedModel;
    static LandScape* landscape;
	static Model* LandscapeModel;
    static Model* TreeModel;
    static std::vector<float*> guymodels; 


   private:
    static std::vector<Model*> models_;
    static int num_verts_;
    static int current_dude;
};
