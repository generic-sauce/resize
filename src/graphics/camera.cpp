#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

bool Lense::isPerspective()
{
	try {
		std::get<Perspective>(m_projection);
		return true;
	} catch(const std::bad_variant_access&) {
		return false;
	}
}

bool Lense::isOrtho()
{
	try {
		std::get<Ortho>(m_projection);
		return true;
	} catch(const std::bad_variant_access&) {
		return false;
	}
}

std::optional<Lense::Perspective*> Lense::perspective()
{
	if (isPerspective())
		return &std::get<Perspective>(m_projection);
	return {};
}

std::optional<Lense::Ortho*> Lense::ortho()
{
	if (isPerspective())
		return &std::get<Ortho>(m_projection);
	return {};
}

void Lense::setPerspective(float fov, float aspectRatio, float near, float far)
{
	m_projection = Perspective{fov, aspectRatio, near, far};
}

void Lense::setOrtho(float left, float right, float up, float bottom, float near, float far)
{
	m_projection = Ortho{left, right, up, bottom, near, far};
}

glm::vec3 Camera::dir()
{
	try {
		return std::get<glm::vec3>(m_dir);
	} catch(const std::bad_variant_access&) {
		auto angles = std::get<Angles>(m_dir);
		return glm::vec3(
				std::cos(angles.vertical) * std::sin(angles.horizontal),
				std::sin(angles.vertical),
				std::cos(angles.vertical) * std::cos(angles.horizontal));
	}
}

glm::vec3 Camera::right()
{
	return glm::cross(dir(), up());
}

glm::vec3 Camera::up()
{
	glm::vec3 dir = Camera::dir();
	return glm::normalize(glm::cross(glm::vec3(1, 0, 0), dir));
}

void Camera::setDir(glm::vec3 dir)
{
	m_dir = dir;
}

void Camera::setDir(float horizontalAngle, float verticalAngle)
{
	m_dir = Angles{horizontalAngle, verticalAngle};
}

glm::mat4 Camera::matrix()
{
	glm::vec3 dir = Camera::dir();

	glm::vec3 up;
	try {
		// TODO: implement up calculation
		up = glm::vec3(0, 1, 0);
	} catch(const std::bad_variant_access&) {
		auto angles = std::get<Angles>(m_dir);
		glm::vec3 right = glm::vec3(
				std::sin(angles.horizontal - 3.14f/2.0f),
				0,
				std::cos(angles.horizontal - 3.14f/2.0f));
		up = glm::cross(right, dir);
	}

	return glm::lookAt(m_pos, m_pos + dir, up);
}

glm::mat4 Lense::matrix()
{
	if (auto p_ = perspective()) {
		auto p = *p_;
		return glm::perspective(glm::radians(p->fov), p->aspectRatio, p->near, p->far);
	} else if (auto p_ = ortho()) {
		auto p = *p_;
		return glm::ortho(p->left, p->right, p->top, p->bottom, p->near, p->far);
	} else
		return glm::mat4(1.f);
}

