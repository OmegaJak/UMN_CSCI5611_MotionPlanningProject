#pragma once
#include <detail/type_vec3.hpp>
#include <tuple>
#include <vector>
#include "glad.h"

struct LineIndexRange {
    int firstIndex;
    int lastIndex;
};

struct Vertex {
    GLfloat x, y, z;
    GLfloat r, g, b;
};

class DebugManager {
   public:
    static bool ShouldRenderDebugLines;

    static void InitVBO();
    static void Draw();

    // This must be called before ShaderManager initializes (and this creates the VBO) to draw debug lines
    static LineIndexRange RequestLines(int numLines);

    static void SetLine(int lineIndex, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    static void SetLineStart(int lineIndex, const glm::vec3& start);
    static void SetLineEnd(int lineIndex, const glm::vec3& end);
    static void SetLineColor(int lineIndex, const glm::vec3& color);

   private:
    static void SetVertexPosition(int vertexIndex, const glm::vec3& pos);
    static void SetVertexColor(int vertexIndex, const glm::vec3& color);
    static int LineBaseVertexIndex(int lineIndex);
    static int NumVertices();

    static int _numLines;
    static bool _mustReuploadData;
    static std::vector<Vertex> _lineData;
};
