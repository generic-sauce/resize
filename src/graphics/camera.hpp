#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <variant>
#include <optional>

class Lense {
public:
struct Perspective {
		float fov;
		float aspectRatio;
		float near;
		float far;
	};

	struct Ortho {
		float left;
		float right;
		float top;
		float bottom;
		float near;
		float far;
	};

	bool isPerspective();
	bool isOrtho();
	std::optional<Perspective*> perspective();
	std::optional<Ortho*> ortho();
	void setPerspective(float fov, float aspectRatio, float near = 0.1f, float far = 100.f);
	void setOrtho(float left, float right, float top, float bottom, float near = 0.1f, float far = 100.f);

	glm::mat4 matrix();

private:
	std::variant<Perspective, Ortho> m_projection;
};

class Camera {
public:
	Lense m_lense;
	glm::vec3 m_pos;

	glm::vec3 dir();
	glm::vec3 right();
	glm::vec3 up();

	void setDir(glm::vec3 dir);
	void setDir(float horizontalAngle, float verticalAngle);
	glm::mat4 matrix();

private:
	struct Angles {
		float horizontal;
		float vertical;
	};

	std::variant<glm::vec3, Angles> m_dir;
};

#endif
