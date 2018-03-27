#include "gameinputcontroller.hpp"
#include <graphics/graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

glm::vec2 g_cursorPos;
void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	g_cursorPos = glm::vec2(x, y);
}

std::map<int, int> g_keyStates;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	g_keyStates[key] = action;
}

int getKey(int key) {
	auto it = g_keyStates.find(key);
	if (it == std::end(g_keyStates))
		return GLFW_RELEASE;
	return it->second;
}

GameInputController::GameInputController(Graphics* graphics, GameWorld* gameWorld)
	: m_camera(graphics->m_camera)
	, m_graphics(graphics)
	, m_gameWorld(gameWorld)
	, m_horizontalAngle(0)
	, m_verticalAngle(0)
	, m_cursorSpeed(.1f)
	, m_moveSpeed(1)
{
	glfwSetCursorPosCallback(graphics->window(), cursorPosCallback);
	glfwSetInputMode(graphics->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(graphics->window(), keyCallback);
}

#include <iostream>
void GameInputController::update(std::chrono::milliseconds deltaTime)
{
	const auto size = m_graphics->windowSize();
	static auto prevCursorPos = glm::vec2(size.x / 2.f, size.y / 2.f);
	const auto cursorOffset = prevCursorPos - g_cursorPos;

	m_horizontalAngle += cursorOffset.x * m_cursorSpeed * deltaTime.count() / 1000.f;
	m_verticalAngle   += cursorOffset.y * m_cursorSpeed * deltaTime.count() / 1000.f;
	prevCursorPos = g_cursorPos;

	m_camera->setDir(m_horizontalAngle, m_verticalAngle);

	if (glfwGetKey(m_graphics->window(), GLFW_KEY_W))
		m_camera->m_pos += m_camera->dir() * m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_S))
		m_camera->m_pos -= m_camera->dir() * m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_A))
		m_camera->m_pos -= m_camera->right() * m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_D))
		m_camera->m_pos += m_camera->right() * m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_SPACE))
		m_camera->m_pos += m_camera->up() * m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_LEFT_SHIFT))
		m_camera->m_pos -= m_camera->up()* m_moveSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_Q))
		m_horizontalAngle += m_cursorSpeed;
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_E))
		m_horizontalAngle -= m_cursorSpeed;

	if (glfwGetKey(m_graphics->window(), GLFW_KEY_1)) {
		m_horizontalAngle = 0.f;
		m_verticalAngle = 0.f;
	}
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_2)) {
		m_horizontalAngle = 3.14f;
		m_verticalAngle = 0.f;
	}
	if (glfwGetKey(m_graphics->window(), GLFW_KEY_3)) {
		m_camera->m_pos = glm::vec3(1, 1, 10);
	}

	const auto parallUp = m_camera->up() * 10.f;
	const auto parallRight = m_camera->right() * 10.f;
	const auto parallPos = m_camera->m_pos
		+ m_camera->dir() * 10.f
		- parallRight / 2.f
		- parallUp / 2.f;

	if (getKey(GLFW_KEY_F) == GLFW_PRESS) {
		g_keyStates[GLFW_KEY_F] = GLFW_REPEAT;

		std::cout << "+ face" << std::endl;
		m_gameWorld->m_paralls.push_back({parallPos, parallRight, parallUp});
	}

	if (getKey(GLFW_KEY_G) == GLFW_PRESS) {
		g_keyStates[GLFW_KEY_G] = GLFW_REPEAT;

		static std::optional<Parall> portal;

		if (portal) {
			std::cout << "+ portal & link" << std::endl;
			m_gameWorld->m_portals.push_back(portal.value());
			m_gameWorld->m_portals.push_back({parallPos, parallRight, parallUp});
			m_gameWorld->m_portalLinks[m_gameWorld->m_portals.size() - 1] = m_gameWorld->m_portals.size() - 2;
			m_gameWorld->m_portalLinks[m_gameWorld->m_portals.size() - 2] = m_gameWorld->m_portals.size() - 1;
			portal = {};
		} else {
			std::cout << "+ portal" << std::endl;
			portal = Parall{parallPos, parallRight, parallUp};
		}
	}
}
