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

	Window window("OpenGL Boilerplate");
	window.setKeycallback(key_callback);



	//===== SHADER INIT =====
	ShaderLoader shaderLoader;

	std::unique_ptr<Shader> renderShader = shaderLoader.CreateShaders();
	shaderLoader.CompileShaders("Shaders/main.vert", renderShader->m_vertexShaderID);
	shaderLoader.CompileShaders("Shaders/main.frag", renderShader->m_fragmentShaderID);

	shaderLoader.AttachShaders(*renderShader);
	shaderLoader.LinkProgram(*renderShader);

	Object obj;
	loadObject("Objects/quad.obj", obj);
	Model quadModel(&obj, false);

	double startTime = glfwGetTime();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	while (window.Open()) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat timeSinceStart = (float(glfwGetTime()) - float(startTime));
		shaderLoader.SendUniformData("time", timeSinceStart);

		quadModel.Render();

		window.Update();
	}

	shaderLoader.DetachShaders(*renderShader);

	shaderLoader.DestroyShaders(*renderShader);
	shaderLoader.DestroyProgram(*renderShader);

	return 0;
}