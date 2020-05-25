#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>


class Shader {
    unsigned int id;
    unsigned int count;
    unsigned int CompileShader(unsigned int type, const std::string& source);
    std::string ReadFile(const std::string& path);
public:
    void init(const std::string& vertPath, const std::string& fragPath);
    void Bind();
    void Unbind();
};

class VertexBuffer {
    unsigned int id;
    unsigned int count;
public:
    void init(float* vert, uint32_t size);
    void Bind();
    void Unbind();
    void Delete();
};