#pragma once

#include <string>
#include <glad/glad.h>
#include <iostream>
#include <fstream>



// Utitlity class to load, compile and attach a vertex- and fragment shader to a program
class Shader
{

private: 
	unsigned int id;

	unsigned int GetCompiledShader(unsigned int shader_type, const std::string& shader_source);

public:
	Shader() {}
	~Shader() {}

	// Load a vertex and a fragment shader from file
	bool Load(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

	// Use the program
	void Use();
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
	// Delete the program
	void Delete();



};

