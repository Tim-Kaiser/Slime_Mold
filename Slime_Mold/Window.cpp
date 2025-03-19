#include "Window.h"


void window_close_callback(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

Window::Window(std::string title):
	m_title(title),
	m_frames(0),
	m_lastFrametime(0.0f)
{
	if (!glfwInit())
	{
		// Initialization failed
	}
	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	m_width = mode->width;
	m_height = mode->height;

	m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), monitor, NULL);

	if (!&m_window) {
		std::printf("Error creating window!");
	};

	glfwSetWindowCloseCallback(m_window, window_close_callback);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);

	glfwMakeContextCurrent(m_window);

	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	glViewport(0, 0, m_width, m_height);
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


void Window::getSize(int* width, int* height)
{
	glfwGetWindowSize(m_window, width, height);
}

void Window::Update()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	// FRAME COUNT
	if (m_frames % 60 == 0)
	{
		float frametime = (float)glfwGetTime() - m_lastFrametime;
		float fps = 1.0f / frametime;

		m_title = std::format("FPS: {}       Frametime in MS: {}", fps, frametime * 1000.0f);
		glfwSetWindowTitle(m_window, m_title.c_str());
	}

	m_lastFrametime = (float)glfwGetTime();
	m_frames++;

}

bool Window::Open()
{
	return !glfwWindowShouldClose(m_window);
}

GLFWwindow* Window::getWindow()
{
	return m_window;
}

void Window::setKeycallback(GLFWkeyfun callback)
{
	glfwSetKeyCallback(m_window, callback);
}

void Window::setMousecallback(GLFWcursorposfun callback)
{
	glfwSetCursorPosCallback(m_window, callback);
}
