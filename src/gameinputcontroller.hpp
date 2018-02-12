#ifndef GAMEINPUTCONTROLLER_HPP
#define GAMEINPUTCONTROLLER_HPP

#include <chrono>
#include <memory>
#include <graphics/camera.hpp>

class GameInputController {
public:
	GameInputController(class Graphics* graphics);

	void update(std::chrono::milliseconds deltaTime);

	std::shared_ptr<Camera> m_camera;

private:
	float m_horizontalAngle;
	float m_verticalAngle;
	float m_cursorSpeed;
};

#endif
