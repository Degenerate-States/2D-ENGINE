#include "shader.h"
    /* TEMP from engine.cpp
    // 4 x 2
    double* test2 = (double*)(void*)assets->plrAsset.data();
        for(int x; x < assets->plrAsset.size()*2; x++) {
        cout << test2[x] << endl;
    }

    //
    float verts[6] = {
        -0.5f, -0.5f, 
        0.0f, 0.5f, 
        0.5f, -0.5f
    };

    VertexBuffer* testBuffer = new VertexBuffer();
    testBuffer->init(verts, 6 * sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); 

    Shader* testShader = new Shader();
    testShader->init("assets/shaders/test.vert", "assets/shaders/test.frag");
    //testShader->Bind();
    */


unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {

    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);


    // errors
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int len;
        char msg[512];
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        glGetShaderInfoLog(id, 512, &len, msg);
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;
        std::cerr << msg << std::endl;
    }

    return id;
}

std::string Shader::ReadFile(const std::string& path) {
    std::string result;
    std::ifstream in(path, std::ios::in);
    if (in) {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1) {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
            in.close();
        }
        else {
            std::cerr << "Provided shader file is empty" << std::endl;
        }
    }
    else {
        std::cerr << "Shader file: "<< path << "does not exist" << std::endl;
    }
    return result;
}

void Shader::init(const std::string& vertPath, const std::string& fragPath) {
    id = glCreateProgram();
    std::string vertShader = ReadFile(vertPath);
    std::string fragShader = ReadFile(fragPath);
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

    glAttachShader(id, vs);
    glAttachShader(id, fs);

    glLinkProgram(id);
    glValidateProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);

}

void Shader::Bind() {
    glUseProgram(id);
}

void Shader::Unbind() {
    glUseProgram(0);
}


// Vertex Buffer
void VertexBuffer::init(float* vert, uint32_t size) {
    glCreateBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, vert, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Delete() {
    glDeleteBuffers(1, &id);
}