#pragma once
#include <glm.hpp>
#include <string>
#include <vector>
#include "Model.h"
#include "Utils.h"

class LandScape {
   public:
	  LandScape(int width, int depth, int height) {
        float* model_;
        int num_verts_ = 0;
        printf("Starting Load Landscape: \n\n");
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

		points = new glm::vec3*[width];
        for (int i = 0; i < width; i++) {
            points[i] = new glm::vec3[depth];
        }
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < depth; j++) {
                points[i][j] = glm::vec3(i, j, Utils::randBetween(0, height));
            }
        }

        printf("Made points: ");
        for (int i = 1; i < width - 1; i++) {  // Smoothens the terrain out by averaging nearby points together
            for (int j = 1; j < depth - 1; j++) {
                float h1 = points[i][j][2];
                float h2 = points[i - 1][j][2] + points[i + 1][j][2] + points[i][j - 1][2] + points[i][j + 1][2];
                float h3 = points[i - 1][j - 1][2] + points[i + 1][j - 1][2] + points[i - 1][j + 1][2] + points[i + 1][j + 1][2];
                points[i][j][2] = h1 / 4 + h2 / 8 + h3 / 16;
            }
        }

        printf(" Smoothed triangles");
        for (int i = 1; i < width; i++) {
            for (int j = 0; j < depth - 1; j++) {  // Adds triangles between all generated points at the aprpriate places
                positions.push_back(points[i][j]);
                positions.push_back(points[i - 1][j + 1]);
                positions.push_back(points[i][j + 1]);
                positions.push_back(points[i][j]);
                positions.push_back(points[i - 1][j]);
                positions.push_back(points[i - 1][j + 1]);
                uvs.push_back(glm::vec2(1, 0));
                uvs.push_back(glm::vec2(0, 1));
                uvs.push_back(glm::vec2(1, 1));
                uvs.push_back(glm::vec2(1, 0));
                uvs.push_back(glm::vec2(0, 0));
                uvs.push_back(glm::vec2(0, 1));
                glm::vec3 norm1 = -1.f * glm::cross(points[i - 1][j + 1] - points[i][j], points[i][j + 1] - points[i][j]);
                glm::vec3 norm2 = -1.f * glm::cross(points[i - 1][j] - points[i][j], points[i - 1][j + 1] - points[i - 1][j]);

                normals.push_back(norm1);
                normals.push_back(norm1);
                normals.push_back(norm1);
                normals.push_back(norm2);
                normals.push_back(norm2);
                normals.push_back(norm2);
            }
        }
        int numverts = positions.size();
        printf("Made Triangse");
        model_ = new float[numverts * 8];
        for (int i = 0; i < numverts; i++) {
            model_[i * 8 + POSITION_OFFSET] = positions[i].x * 3.f;
            model_[i * 8 + POSITION_OFFSET + 1] = positions[i].y * 3.f;
            model_[i * 8 + POSITION_OFFSET + 2] = positions[i].z * 3.f;
            model_[i * 8 + TEXCOORD_OFFSET] = uvs[i].x;
            model_[i * 8 + TEXCOORD_OFFSET + 1] = uvs[i].y;
            model_[i * 8 + NORMAL_OFFSET] = normals[i].x;
            model_[i * 8 + NORMAL_OFFSET + 1] = normals[i].y;
            model_[i * 8 + NORMAL_OFFSET + 2] = normals[i].z;
        }
        printf("FINISHED");
        num_verts_ = numverts;
        model = new Model(model_, num_verts_);
	}

float get_height_at(glm::vec3 point) {
            // Every point falls into a square
            // The following 12 Lines determine which triangle of
            // That square the object is in
            glm::vec3 v1;
            glm::vec3 v2;
            glm::vec3 normal;
            int xf = floor(point[0]);
            int zf = floor(point[1]);
            int x1 = xf;
            int z1 = zf;
            int x2 = x1 + 1;
            int z2 = z1 + 1;
            float x3 = (round(point[0])) - floor(round(point[0]));
            float z3 = (round(point[1])) - floor(round(point[1]));




            if ((x3 >= -z3 + 1)) {                     // Using the equation x >= -z + 1 to determine which triangle the object is in.
                v1 = points[x1][z2] - points[x2][z1];  // Top Triangle
                v2 = points[x2][z2] - points[x2][z1];
               // printf("Top");
            } else {
                v1 = points[x1][z2] - points[x2][z1];  // Bottom Triangle
                v2 = points[x1][z1] - points[x1][z2];
               // printf("Bottom");
            }

            normal = glm::cross(v1, v2);
            // Get normal to the plane

            normal = glm::normalize(normal);

            // val is the d value in the plane equation ax + by + cz = d
            float val = normal[0] * points[x1][z2][0] + normal[1] * points[x1][z2][1] + normal[2] * points[x1][z2][2];
            float res = get_plane_height(normal, val, point);
            //res = points[x1][z2][2];
            //printf("Height at: x: %f, y: %f, z: %f, is: %f\n\n\n", point.x, point.y, point.z, res);
            return res;
        }

        float get_plane_height(glm::vec3 normal, float val, glm::vec3 point) {
            float result = 0;  // solves for y. : y = (d - ax - cz) / b
            result = (val - normal[0] * (point[0]) - normal[1] * (point[1])) / normal[2];
            return absolute(result);
        }

        float absolute(float input) {  // Absolute value function, since Linux's only works on ints.
            if (input < -0) {
                return -1 * input;
            } else
                return input;
        }

    Model* model;
    int width, height, depth;
    glm::vec3** points;
};


