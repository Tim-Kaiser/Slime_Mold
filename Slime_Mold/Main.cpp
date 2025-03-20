#define GLFW_INCLUDE_NONE

#include "Window.h"
#include "ShaderLoader.h"
#include "ObjectLoader.h"
#include "Model.h"

#include <vector>
#include <glm.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(int argc, char* arfv[]) {

	Window window("Slime Mold");
	window.setKeycallback(key_callback);



	//===== SHADER INIT =====
	ShaderLoader shaderLoader;

	std::unique_ptr<Shader> renderShader = shaderLoader.CreateShaders();
	shaderLoader.CompileShaders("Shaders/main.vert", renderShader->m_vertexShaderID);
	shaderLoader.CompileShaders("Shaders/main.frag", renderShader->m_fragmentShaderID);

	shaderLoader.AttachShaders(*renderShader);
	shaderLoader.LinkProgram(renderShader->m_shaderProgramID);

	std::unique_ptr<ComputeShader> computeShader = shaderLoader.CreateComputeShader();
	shaderLoader.CompileShaders("Shaders/slime_mold.comp", computeShader->m_computeShaderID);
	shaderLoader.AttachShaders(*computeShader);
	shaderLoader.LinkProgram(computeShader->m_shaderProgramID);

	Object obj;
	loadObject("Objects/quad.obj", obj);
	Model quadModel(&obj, false);



	// IMG TEXTURE FOR COMPUTE SHADER

	int width, height;
	window.getSize(&width, &height);

	GLuint texID;

	glGenTextures(1, &texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
		GL_FLOAT, NULL);

	glBindImageTexture(0, texID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	double startTime = glfwGetTime();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	while (window.Open()) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(computeShader->m_shaderProgramID);
		glDispatchCompute((GLuint)width, (GLuint)height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//GLfloat timeSinceStart = (float(glfwGetTime()) - float(startTime));
		//shaderLoader.SendUniformData("time", timeSinceStart);

		glUseProgram(renderShader->m_shaderProgramID);		
		glActiveTexture(GL_TEXTURE0);
		GLint id = 0;
		shaderLoader.SendUniformData("tex", id);
		glBindTexture(GL_TEXTURE_2D, texID);
		quadModel.Render();

		window.Update();
	}

	// === SHADER CLEAN UP ===

	shaderLoader.DetachShaders(*renderShader);

	shaderLoader.DestroyShaders(*renderShader);
	shaderLoader.DestroyProgram(renderShader->m_shaderProgramID);

	return 0;
}