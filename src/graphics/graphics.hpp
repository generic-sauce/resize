#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "camera.hpp"
#include "renderer.hpp"

class GameWorld;

class Graphics {
public:
	Graphics();
	~Graphics();

	bool init();
	void update();
	void render(GameWorld* gameWorld);
	GLFWwindow* window();
	glm::uvec2 windowSize();

	std::shared_ptr<Camera> m_camera;
	Renderer m_renderer;

private:
	GLFWwindow* m_window;
	glm::uvec2 m_size;
};

#endif
