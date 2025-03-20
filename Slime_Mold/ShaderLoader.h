#pragma once

#include <string>
#include "include/glad/gl.h"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <memory> 


typedef struct
{
	GLuint m_shaderProgramID;
	GLuint m_vertexShaderID;
	GLuint m_fragmentShaderID;

} Shader;

typedef struct 
{
	GLuint m_shaderProgramID;
	GLuint m_computeShaderID;
} ComputeShader;

class ShaderLoader {

public:

	// === SHADER ===

	std::unique_ptr<Shader> CreateShaders();

	bool CompileShaders(const std::string& filename, GLuint& shaderID);
	void AttachShaders(Shader& shader);
	bool LinkProgram(GLuint& shaderProgramID);


	void DetachShaders(Shader& shader);
	void DestroyShaders(Shader& shader);
	void DestroyProgram(GLuint& shaderProgramID);
	
	// === COMPUTE SHADER ===

	std::unique_ptr<ComputeShader> CreateComputeShader();

	void AttachShaders(ComputeShader& shader);

	void DetachShaders(ComputeShader& shader);
	void DestroyShaders(ComputeShader& shader);


	GLint GetUniformID(const std::string& uniformName);

	bool SendUniformData(const std::string& uniformName, GLint& data);
	bool SendUniformData(const std::string& uniformName, GLfloat& data);
	bool SendUniformData(const std::string& uniformName, GLuint& data);

	bool SendUniformData(const std::string& uniformName, GLfloat& x, GLfloat& y);
	bool SendUniformData(const std::string& uniformName, GLfloat& x, GLfloat& y, GLfloat& z);
	bool SendUniformData(const std::string& uniformName, GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& w);

	bool SendUniformData(const std::string& uniformName, const glm::mat4& data);
	bool SendUniformData(const std::string& uniformName, const glm::vec3& data);
};