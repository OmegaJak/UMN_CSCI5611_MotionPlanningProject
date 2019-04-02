#pragma once
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glm.hpp>
#include <string>
#include <vector>


class Model {
   public:
    explicit Model(const std::string& file);

    void LoadTxt(const std::string& file);
    void LoadObj(const std::string& file);
    void LoadObj2(const std::string& file);
    void LoadDae(const std::string& file);

	static void print_xmlfile(xmlNode* a_node, int level);
    static xmlNode* searchNode(xmlNode* a_node, char target[]);
    static void parseNode(xmlDocPtr doc, xmlNodePtr cur);

    int NumElements() const;
    int NumVerts() const;

    std::vector<glm::vec4> Vertices() const;

    float* model_;
    int vbo_vertex_start_index_;

   private:
    int num_verts_;
};


