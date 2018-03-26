#include <iostream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "graphics/graphics.hpp"
#include "gameinputcontroller.hpp"
#include "graphics/renderer.hpp"
#include "gameworld.hpp"
//#include "game/game.hpp"

int main()
{
	Graphics graphics;
	if (!graphics.init())
		return 0;

	graphics.m_camera = std::shared_ptr<Camera>(new Camera());
	graphics.m_camera->m_pos = glm::vec3(5, 5, 16);
	graphics.m_camera->setDir(glm::vec3(0, 0.00001f, -1));
	graphics.m_camera->m_lense.setPerspective(65.f, 1.f, .1f, 1000.f);

	GameInputController gameInputController(&graphics);
	//gameInputController.m_camera = graphics.m_camera;

	GameWorld gameWorld;

	//Game game;
	//if (!game.init())
	//	return 0;

	std::cout << "running game..." << std::endl;
	using namespace std::chrono_literals;
	std::chrono::steady_clock clock;
	auto beginTime = clock.now();
	auto prevTime = beginTime;

	const float speed = 1.f;

	while (!glfwWindowShouldClose(graphics.window())) {
		auto curTime = clock.now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime);
		prevTime = curTime;

		glfwPollEvents();
		if (glfwGetKey(graphics.window(), GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(graphics.window(), true);

		auto dir = graphics.m_camera->dir();
		auto up = glm::vec3(0, 1, 0);
		auto right = glm::cross(dir, up);

		if (glfwGetKey(graphics.window(), GLFW_KEY_W))
			graphics.m_camera->m_pos += dir * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_S))
			graphics.m_camera->m_pos -= dir * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_A))
			graphics.m_camera->m_pos -= right * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_D))
			graphics.m_camera->m_pos += right * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_SPACE))
			graphics.m_camera->m_pos += up * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_LEFT_SHIFT))
			graphics.m_camera->m_pos -= up * speed;
		if (glfwGetKey(graphics.window(), GLFW_KEY_Q))
			graphics.m_camera->setDir(glm::rotate(dir,  speed * glm::pi<float>() / 32.f, up));
		if (glfwGetKey(graphics.window(), GLFW_KEY_E))
			graphics.m_camera->setDir(glm::rotate(dir, -speed * glm::pi<float>() / 32.f, up));

		//graphics.m_camera->m_pos.z += std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() / 600.f;
		gameInputController.update(deltaTime);
		//game.update(deltaTime);
		
		graphics.update();
		graphics.render(&gameWorld);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(50ms);

		//std::cout << getchar() << std::endl;
	}

	return 0;
}
