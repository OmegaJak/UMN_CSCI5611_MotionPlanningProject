#define _CRT_SECURE_NO_WARNINGS

#include <detail/type_vec3.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "Constants.h"
#include "model.h"
#include "ModelManager.h"

using std::string;
using std::vector;

Model::Model(const string& file) {
    unsigned int dot_position = file.find_last_of('.');
    if (dot_position == string::npos) {
        printf("Given file \"%s\" did not have an extension. Exiting...\n", file.c_str());
        exit(1);
    }

    string extension = file.substr(dot_position + 1, 3);
    if (extension == "txt") {
        LoadTxt(file);
    } else if (extension == "obj") {
        LoadObj2(file);
    } else if (extension == "dae") {
        LoadDae(file);
	} else {
        printf("Unrecognized file extension \"%s\" for file \"%s\". Exiting...\n", extension.c_str(), file.c_str());
        exit(1);
    }

    ModelManager::RegisterModel(this);
}

void Model::LoadTxt(const std::string& file) {
    std::ifstream modelFile;
    modelFile.open(file);
    int num_elements;
    modelFile >> num_elements;
    model_ = new float[num_elements];

    for (int i = 0; i < num_elements; i++) {
        modelFile >> model_[i];
    }

    printf("%d\n", num_elements);
    num_verts_ = num_elements / ELEMENTS_PER_VERT;
    modelFile.close();
}

// Credit for the basis of this methodology goes to http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// This only supports an obj defining vertex positions, normals, and triangles
void Model::LoadObj(const std::string& filename) {
    vector<unsigned int> vertex_indices, normal_indices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec3> temp_normals;

    FILE* file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        printf("Failed to open file \"%s\". Exiting...\n", filename.c_str());
        exit(1);
    }

    while (true) {
        char line_header[128];

        // read the first word of the line
        int res = fscanf(file, "%s", line_header);
        if (res == EOF) break;  // End of file. Leave loop

        if (strcmp(line_header, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(line_header, "vt") == 0) {
            glm::vec3 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
        } else if (strcmp(line_header, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(line_header, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertex_index[3], normal_index[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertex_index[0], &normal_index[0], &vertex_index[1], &normal_index[1],
                                 &vertex_index[2], &normal_index[2]);
            if (matches != 6) {
                printf("File \"%s\" can't be read by this simple parser. Exiting...\n", filename.c_str());
                exit(1);
            }

            vertex_indices.push_back(vertex_index[0]);
            vertex_indices.push_back(vertex_index[1]);
            vertex_indices.push_back(vertex_index[2]);
            normal_indices.push_back(normal_index[0]);
            normal_indices.push_back(normal_index[1]);
            normal_indices.push_back(normal_index[2]);
        }
    }

    // We've parsed the data into vectors, so now transform the vector data into OpenGL data
    if (vertex_indices.size() % 3 != 0) {
        printf("The number of vertex indices was not a factor of 3. Exiting...");
        exit(1);
    }

    int num_verts = vertex_indices.size();
    int model_size = num_verts * ELEMENTS_PER_VERT;
    model_ = new float[model_size];
    for (int vertex_number = 0; vertex_number < num_verts; vertex_number++) {
        int vertex_index = vertex_indices[vertex_number];
        glm::vec3 vertex = temp_vertices[vertex_index - 1];  // subtract 1 because objs are indexed from 1, not 0

        int vertex_offset = vertex_number * ELEMENTS_PER_VERT + POSITION_OFFSET;
        model_[vertex_offset] = vertex.x;
        model_[vertex_offset + 1] = vertex.y;
        model_[vertex_offset + 2] = vertex.z;

        int normal_index = normal_indices[vertex_number];
        glm::vec3 normal = temp_normals[normal_index - 1];  // subtract 1 because objs are indexed from 1

        int normal_offset = vertex_number * ELEMENTS_PER_VERT + NORMAL_OFFSET;
        model_[normal_offset] = normal.x;
        model_[normal_offset + 1] = normal.y;
        model_[normal_offset + 2] = normal.z;

        int uv_offset = vertex_number * ELEMENTS_PER_VERT + TEXCOORD_OFFSET;
        model_[uv_offset] = model_[uv_offset + 1] = 0;  // Set uv's to zero because this parser doesn't handle them and they're expected
    }

    num_verts_ = num_verts;
    fclose(file);
}

void Model::LoadObj2(const std::string& filename) {
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;
    std::vector<unsigned int> indices, uvs, normals;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::ifstream modelFile;
    modelFile.open(filename);
    int numLines = 0;
    string command;
    string unused_line;
    while (modelFile >> command) {  // Read first word in the line (i.e., the command type)

        if (command[0] == '#') {
            getline(modelFile, unused_line);  // skip rest of line
            std::cout << "Skipping comment: " << command << unused_line << std::endl;
            continue;
        } else if (command == "v") {
            glm::vec3 vertex;
            float x, y, z;
            modelFile >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        } else if (command == "vt") {
            glm::vec2 uv;
            modelFile >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        } else if (command == "vn") {
            glm::vec3 normal;
            modelFile >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        } else if (command == "f") {
            std::string v1, v2, v3;
            unsigned int vIndex[3], uIndex[3], nIndex[3];
            for (int i = 0; i < 3; i++) {
                modelFile >> vIndex[i];
                modelFile.ignore(1);
                modelFile >> uIndex[i];
                modelFile.ignore(1);
                modelFile >> nIndex[i];
            }

            for (int i = 0; i < 3; i++) {
                indices.push_back(vIndex[i]);
                uvs.push_back(uIndex[i]);
                normals.push_back(nIndex[i]);
            }
        } else {
            getline(modelFile, unused_line);  // skip rest of line
            std::cout << "WARNING. Do not know command: " << command << std::endl;
        }
    }

    for (int i = 0; i < indices.size(); i++) {
        int index = indices[i];
        glm::vec3 vertex = temp_vertices[index - 1];  // Obj has index starting at 1
        out_vertices.push_back(vertex);
    }
    for (int i = 0; i < uvs.size(); i++) {
        int index = uvs[i];
        glm::vec2 uv = temp_uvs[index - 1];
        out_uvs.push_back(uv);
    }
    for (int i = 0; i < normals.size(); i++) {
        int index = normals[i];
        glm::vec3 normal = temp_normals[index - 1];
        out_normals.push_back(normal);
    }
    printf("Loading parsed data into vectors");
    printf("normal size: %d", out_vertices.size());
    printf(" vertex size: %d", out_uvs.size());
    printf(" indice size: %d", out_normals.size());
    printf("\n");

    model_ = new float[out_vertices.size() * 8];
    for (int i = 0; i < out_vertices.size(); i++) {
        model_[i * 8 + POSITION_OFFSET] = out_vertices[i].x;
        model_[i * 8 + POSITION_OFFSET + 1] = out_vertices[i].y;
        model_[i * 8 + POSITION_OFFSET + 2] = out_vertices[i].z;
        model_[i * 8 + TEXCOORD_OFFSET] = out_uvs[i].x;
        model_[i * 8 + TEXCOORD_OFFSET + 1] = out_uvs[i].y;
        model_[i * 8 + NORMAL_OFFSET] = out_normals[i].x;
        model_[i * 8 + NORMAL_OFFSET + 1] = out_normals[i].y;
        model_[i * 8 + NORMAL_OFFSET + 2] = out_normals[i].z;
        
    }
    num_verts_ = out_vertices.size();
    modelFile.close();
}

void Model::LoadDae(const std::string& filename) {
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;
    std::vector<unsigned int> indices, uvs, normals;
    std::vector<float> data;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

	xmlDoc* doc = NULL;
	xmlNode* root = NULL;
	LIBXML_TEST_VERSION

	doc = xmlReadFile(filename.c_str(), NULL, 0);
	if (doc == NULL) {
		printf("Error: could not parse file: %s\n", filename);
	}
	root = xmlDocGetRootElement(doc);
	//print_xmlfile(root, 0);

	xmlNode* nodeFound;
    nodeFound = searchNode(root, (char*)"geometry");
    print_xmlfile(nodeFound->parent, 0);

	if (nodeFound != NULL) {
        nodeFound = searchNode(nodeFound, (char*)"float_array");
	}
    
	if (nodeFound != NULL) {
            xmlChar* key;
            nodeFound = nodeFound->xmlChildrenNode;
            while (nodeFound != NULL) {
                key = xmlNodeListGetString(doc, nodeFound, 1);
                string input = (char*)key;
                std::istringstream ss(input);
                string token;
                printf("-----\n");
                int xyz = 0;
                while (std::getline(ss, token, ' ')) {
                    data.push_back(std::stoi(token));
                }
                printf("-----\n");
                xmlFree(key);
                nodeFound = nodeFound->next;
            }


		//parseNode(doc, nodeFound);
	}
    printf("-------------------------------------------\n");

	xmlFreeDoc(doc);
	xmlCleanupParser();

}

int Model::NumElements() const {
    return num_verts_ * ELEMENTS_PER_VERT;
}

int Model::NumVerts() const {
    return num_verts_;
}

std::vector<glm::vec4> Model::Vertices() const {
    std::vector<glm::vec4> verts;
    for (int i = POSITION_OFFSET; i < NumElements(); i += ATTRIBUTE_STRIDE) {
        verts.push_back(glm::vec4(model_[i], model_[i + 1], model_[i + 2], 1.0));  // These are positions so w=1 for vec4s
    }

    return verts;
}

//Recursively print xml file node names.
//xmlsoft.org/examples/tree1.c
void Model::print_xmlfile(xmlNode* a_node, int level) {
    level++;
	xmlNode* cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
            for (int i = 0; i < level; i++) {
                printf(" +"); // Level
			}
			printf("%s\n", cur_node->name);
		}
		print_xmlfile(cur_node->children, level);
	}
}
//Searches for a node, returns NULL if not found
//http://cse.csusb.edu/tongyu/courses/cs520/notes/cs520b.pdf
xmlNode* Model::searchNode(xmlNode* a_node, char target[]) {
    xmlNode* nodeFound = NULL;
    for (xmlNode* cur = a_node; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur->name, (const xmlChar*)target)) {
                printf("Found %s \n", cur->name);
                nodeFound = cur;
                break;
            }
        }
        // search recursively until node is found.
        if (nodeFound == NULL && cur != NULL) nodeFound = searchNode(cur->children, target);
    }
    return nodeFound;
}

void Model::parseNode(xmlDocPtr doc, xmlNodePtr cur) {

    xmlChar* key;
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        key = xmlNodeListGetString(doc, cur, 1);
        string input = (char*)key;
        std::istringstream ss(input);
        string token;
        printf("-----\n");
        while (std::getline(ss, token, ' ')) {
            std::cout << token << ' ';
		}
        printf("-----\n");
        xmlFree(key);
        cur = cur->next;
    }
    return;
}