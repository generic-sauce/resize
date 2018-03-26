#ifndef GAMEINPUTCONTROLLER_HPP
#define GAMEINPUTCONTROLLER_HPP

#include <chrono>
#include <memory>
#include <graphics/camera.hpp>

class Graphics;
class GameWorld;

class GameInputController {
public:
	GameInputController(Graphics* graphics, GameWorld* gameWorld);

	void update(std::chrono::milliseconds deltaTime);

	std::shared_ptr<Camera> m_camera;

private:
	Graphics* m_graphics;
	GameWorld* m_gameWorld;

	float m_horizontalAngle;
	float m_verticalAngle;
	float m_cursorSpeed;
	float m_moveSpeed;
};

#endif
