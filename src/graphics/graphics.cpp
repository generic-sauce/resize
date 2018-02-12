#include <GL/glew.h>
#include "graphics.hpp"
#include <iostream>

void error_callback(int error, const char* description)
{
	std::cout << error << ": " << description << std::endl;
}

Graphics::Graphics()
	: m_size(800, 600)
{}

Graphics::~Graphics()
{
	if (m_window)
		glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Graphics::init()
{
	std::cout << "Hello Resize!" << std::endl;

	glfwSetErrorCallback(error_callback);
	std::cout << "inizializing glfw..." << std::endl;
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "failed to inizialize glfw" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	std::cout << "creating window..." << std::endl;
	auto* window = glfwCreateWindow(m_size.x, m_size.y, "resize", nullptr, nullptr);
	if (!window) {
		std::cout << "failed to create window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	std::cout << "inizializing glew..." << std::endl;
	if (glewInit() != GLEW_OK) {
		std::cout << "failed to inizialize glew" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	std::cout << "graphics initialized sucessfully" << std::endl;
	m_window = window;

	return m_renderer.init(this);
}

void Graphics::update()
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);

	if (width != m_size.x || height != m_size.y) {
		m_size = glm::uvec2(width, height);

		glViewport(0, 0, width, height);
		m_renderer.resize(m_size);
	}

	if (m_camera) {
		auto optional = m_camera->m_lense.perspective();
		if (optional; auto& perspective = optional.value())
			perspective->aspectRatio = static_cast<float>(width) / height;
	}
}

void Graphics::render(GameWorld *gameWorld)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_renderer.render(this, gameWorld);
	glfwSwapBuffers(m_window);
}

GLFWwindow* Graphics::window()
{
	return m_window;
}

glm::uvec2 Graphics::windowSize()
{
	return m_size;
}
