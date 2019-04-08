#include "DebugManager.h"
#include "ShaderManager.h"

int DebugManager::_numLines = 0;
bool DebugManager::_mustReuploadData = false;
bool DebugManager::ShouldRenderDebugLines = true;
std::vector<Vertex> DebugManager::_lineData = std::vector<Vertex>();

void DebugManager::InitVBO() {
    _lineData.resize(NumVertices());

    glGenBuffers(1, &ShaderManager::DebugShader.VBO);               // Create the vbo
    glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::DebugShader.VBO);  // Set the vbo as the active array buffer
    glBufferData(GL_ARRAY_BUFFER, _lineData.size() * sizeof(Vertex), _lineData.data(), GL_DYNAMIC_DRAW);  // upload vertices to vbo
}

void DebugManager::Draw() {
    if (_mustReuploadData) {
        glBindBuffer(GL_ARRAY_BUFFER, ShaderManager::DebugShader.VBO);  // This should already be bound, no?
        auto data = _lineData.data();
        glBufferSubData(GL_ARRAY_BUFFER, 0, _lineData.size() * sizeof(Vertex), _lineData.data());

        _mustReuploadData = false;
    }

    if (ShouldRenderDebugLines) glDrawArrays(GL_LINES, 0, NumVertices());
}

LineIndexRange DebugManager::RequestLines(int numLines) {
    LineIndexRange range{};
    range.firstIndex = _numLines;

    _numLines += numLines;
    range.lastIndex = _numLines - 1;

    _lineData.resize(NumVertices(), {0, 0, 0, 0, 0, 0});

    return range;
}

void DebugManager::SetLine(int lineIndex, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    SetLineStart(lineIndex, start);
    SetLineEnd(lineIndex, end);
    SetLineColor(lineIndex, color);
}

void DebugManager::SetLineStart(int lineIndex, const glm::vec3& start) {
    SetVertexPosition(LineBaseVertexIndex(lineIndex), start);
}

void DebugManager::SetLineEnd(int lineIndex, const glm::vec3& end) {
    SetVertexPosition(LineBaseVertexIndex(lineIndex) + 1, end);
}

void DebugManager::SetLineColor(int lineIndex, const glm::vec3& color) {
    int vertexIndex = LineBaseVertexIndex(lineIndex);
    SetVertexColor(vertexIndex, color);
    SetVertexColor(vertexIndex + 1, color);
}

void DebugManager::SetVertexPosition(int vertexIndex, const glm::vec3& pos) {
    _lineData[vertexIndex].x = pos.x;
    _lineData[vertexIndex].y = pos.y;
    _lineData[vertexIndex].z = pos.z;

    _mustReuploadData = true;
}

void DebugManager::SetVertexColor(int vertexIndex, const glm::vec3& color) {
    _lineData[vertexIndex].r = color.r;
    _lineData[vertexIndex].g = color.g;
    _lineData[vertexIndex].b = color.b;

    _mustReuploadData = true;
}

int DebugManager::LineBaseVertexIndex(int lineIndex) {
    return lineIndex * 2;
}

int DebugManager::NumVertices() {
    return _numLines * 2;
}
