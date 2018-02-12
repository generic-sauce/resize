#include "gameinputcontroller.hpp"
#include <graphics/graphics.hpp>
#include <glm/glm.hpp>

glm::vec2 g_cursorPos;
void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	g_cursorPos = glm::vec2(x, y);
}

GameInputController::GameInputController(Graphics* graphics)
	: m_horizontalAngle(0)
	, m_verticalAngle(0)
	, m_cursorSpeed(1)
{
	glfwSetCursorPosCallback(graphics->window(), cursorPosCallback);
	glfwSetInputMode(graphics->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GameInputController::update(std::chrono::milliseconds deltaTime)
{
	m_horizontalAngle += g_cursorPos.x * m_cursorSpeed * deltaTime.count();
	m_verticalAngle += g_cursorPos.y * m_cursorSpeed * deltaTime.count();

	if (m_camera)
		m_camera->setDir(m_horizontalAngle, m_verticalAngle);
}
