#define GLFW_INCLUDE_NONE

#include "Window.h"
#include "ShaderLoader.h"
#include "ObjectLoader.h"
#include "Model.h"

#include <vector>
#include <glm.hpp>
#include <random>

#define PARTICLE_NUM 500000
#define CIRCLE 


struct Particle
{
	GLfloat x;
	GLfloat y;
	GLfloat angle; GLfloat __padding;
};

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

	std::unique_ptr<ComputeShader> particleComputeShader = shaderLoader.CreateComputeShader();
	shaderLoader.CompileShaders("Shaders/particle_movement.comp", particleComputeShader->m_computeShaderID);
	shaderLoader.AttachShaders(*particleComputeShader);
	shaderLoader.LinkProgram(particleComputeShader->m_shaderProgramID);

	std::unique_ptr<ComputeShader> trailComputeShader = shaderLoader.CreateComputeShader();
	shaderLoader.CompileShaders("Shaders/trail_processing.comp", trailComputeShader->m_computeShaderID);
	shaderLoader.AttachShaders(*trailComputeShader);
	shaderLoader.LinkProgram(trailComputeShader->m_shaderProgramID);


	Object obj;
	loadObject("Objects/quad.obj", obj);
	Model quadModel(&obj, false);



	// IMG TEXTURE FOR COMPUTE SHADER

	int width, height;
	window.getSize(&width, &height);

	GLuint particleMapID, trailMapID;

	glGenTextures(1, &particleMapID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleMapID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
		GL_FLOAT, NULL);


	glGenTextures(1, &trailMapID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, trailMapID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
		GL_FLOAT, NULL);

	// SSBO for particles


	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> angleInRad(0, 6.2831);

#ifdef CIRCLE

	std::uniform_int_distribution<> circle(0, height/2.5);
#else
	int qW = width / 2;
	int qH = height / 4;

	std::uniform_int_distribution<> dW(1, width);
	std::uniform_int_distribution<> dH(1, height);

#endif

	std::vector<Particle> particles;
	particles.reserve(PARTICLE_NUM);

	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		Particle p;
#ifdef CIRCLE
		float angle = angleInRad(gen);
		int dist = circle(gen);
		p.x = width/2.0f + (cos(angle) * dist);
		p.y = height / 2.0f + (sin(angle) * dist);
		p.angle = angle + 3.14;
#else
		p.x = (GLfloat)dW(gen);
		p.y = (GLfloat)dH(gen);
		p.angle = angleInRad(gen) + 3.14;
#endif
		

		particles.emplace_back(p);
	}

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_NUM * sizeof(Particle), &particles[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// TIMING
	double startTime = glfwGetTime();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	while (window.Open()) {


		glClear(GL_COLOR_BUFFER_BIT);

		// COMPUTE SHADER

		glUseProgram(particleComputeShader->m_shaderProgramID);

		// SETUP
		GLfloat timeSinceStart = (float(glfwGetTime()) - float(startTime));
		shaderLoader.SendUniformData("time", timeSinceStart);
		shaderLoader.SendUniformData("width", width);
		shaderLoader.SendUniformData("height", height);

		glBindImageTexture(0, particleMapID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, trailMapID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

		// RUN
		glDispatchCompute(PARTICLE_NUM, 1, 1);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// TRAIL PROCESSING

		glUseProgram(trailComputeShader->m_shaderProgramID);
		shaderLoader.SendUniformData("width", width);
		shaderLoader.SendUniformData("height", height);

		glBindImageTexture(0, trailMapID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute((GLuint)width, (GLuint)height, 1);


		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		// RENDER SHADER

		glUseProgram(renderShader->m_shaderProgramID);

		// SETUP
		GLint particleShaderID = 0;
		GLint trailShaderID = 1;

		glActiveTexture(GL_TEXTURE0);
		shaderLoader.SendUniformData("particleMap", particleShaderID);
		glBindTexture(GL_TEXTURE_2D, particleMapID);

		glActiveTexture(GL_TEXTURE1);
		shaderLoader.SendUniformData("trailMap", trailShaderID);
		glBindTexture(GL_TEXTURE_2D, trailMapID);

		// RUN
		quadModel.Render();

		// SWAP BUFFERS
		window.Update();
	}

	// === SHADER CLEAN UP ===

	shaderLoader.DetachShaders(*renderShader);

	shaderLoader.DestroyShaders(*renderShader);
	shaderLoader.DestroyProgram(renderShader->m_shaderProgramID);


	return 0;
}