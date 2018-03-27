#include "renderer.hpp"
#include "graphics.hpp"
#include "camera.hpp"
#include "misc.hpp"
#include <shapes.hpp>
#include <gameworld.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <cstring>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

std::ostream& operator<< (std::ostream& os, const glm::vec3& v)
{
	return os << "(" << v.x << " | " << v.y << " | " << v.z << ")";
}

std::ostream& operator<<(std::ostream& os, Camera& camera)
{
	return os << "camera( pos" << camera.m_pos << " dir" << camera.dir() << " )";
}

std::vector<glm::vec3> parallsToTriangles(const std::vector<Parall>& paralls)
{
	std::vector<glm::vec3> positions;
	for (auto& p : paralls) {
		positions.push_back(p.pos);
		positions.push_back(p.pos + p.right);
		positions.push_back(p.pos + p.right + p.up);
		positions.push_back(p.pos);
		positions.push_back(p.pos + p.right + p.up);
		positions.push_back(p.pos + p.up);
	}

	return positions;
}

std::vector<glm::vec3> generateTriangleNormals(const std::vector<glm::vec3>& positions)
{
	std::vector<glm::vec3> normals;
	for (std::size_t i = 0; i < positions.size(); i += 3) {
		auto normal = glm::normalize(glm::cross(
			positions[i + 1] - positions[i], positions[i + 2] - positions[i]));
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}

	return normals;
}

GLuint loadShaderFromSourceCode(GLenum type, const char* sourcecode, int length)
{
	GLuint shaderId = glCreateShader(type);

	glShaderSource(shaderId, 1, &sourcecode, &length);
	glCompileShader(shaderId);

	GLint isCompiled = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		auto errorLog = std::make_unique<GLchar[]>(maxLength);
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Error compiling " << std::endl
			<< &errorLog[0] << std::endl;
		glDeleteShader(shaderId); // Don't leak the shader.
		return 0;
	}

	return shaderId;
}

GLuint loadShaderFromFile(GLenum type, const char* filepath)
{
	std::ifstream fstream;
	fstream.open(filepath);

	if (!fstream.is_open())
	{
		std::cout << "Unable to open file '" << filepath << "'" << std::endl;
		return 0;
	}

	std::stringstream sstream;
	std::string line;
	while (std::getline(fstream, line))
		sstream << line << '\n';
	line = sstream.str();

	GLuint shaderId = loadShaderFromSourceCode(type, line.c_str(), line.length());
	if (!shaderId)
		std::cout << "...with filepath '" << filepath << "'"; 

	return shaderId;
} 

GLuint loadProgram(const std::vector<std::pair<GLenum, const char*>>& shaders)
{
	GLuint programId = glCreateProgram();

	std::vector<GLuint> shaderIds;
	auto deleteShaders = [&] {
		for (const auto& shaderId : shaderIds) {
			glDetachShader(programId, shaderId);
			glDeleteShader(shaderId);
		}
	};

	for (const auto& pair : shaders)
	{
		GLuint shaderId = loadShaderFromFile(pair.first, pair.second);
		if (!shaderId) {
			glDeleteProgram(programId);
			deleteShaders();
			return 0;
		}

		glAttachShader(programId, shaderId);
		shaderIds.push_back(shaderId);
	}

	glLinkProgram(programId);

	GLint isLinked = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar infoLog[maxLength + 1];
		glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);
		infoLog[maxLength] = '\0';
		glDeleteProgram(programId);

		std::cout << infoLog << std::endl;
	}

	deleteShaders();
	return programId;
}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

bool Renderer::init(Graphics* graphics)
{
	m_programId = loadProgram({
		{GL_VERTEX_SHADER, "data/shader/vertex.glsl"},
		{GL_FRAGMENT_SHADER, "data/shader/fragment.glsl"},
		});

	if (!m_programId)
		return false;

	m_simpleProgramId = loadProgram({
		{GL_VERTEX_SHADER, "data/shader/simple_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "data/shader/simple_fragment.glsl"},
		});

	if (!m_simpleProgramId)
		return false;

	// scene buffers
	glCreateVertexArrays(1, &m_sceneVao);
	glGenBuffers(2, &m_sceneVbos[0]);

	glBindVertexArray(m_sceneVao);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_sceneVbos[0]);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_sceneVbos[1]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		nullptr);

	glBindVertexArray(0);

	// portal buffers
	glCreateVertexArrays(1, &m_portalVao);
	glGenBuffers(1, &m_portalVbos[0]);

	glBindVertexArray(m_portalVao);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_portalVbos[0]);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		nullptr);

	/*{ // upload portal triangles
		glm::vec3 portal[] {
			{0, 0, 0},
			{1, 0, 0},
			{1, 1, 0},
			{0, 0, 0},
			{1, 1, 0},
			{0, 1, 0},
		};
		glBindBuffer(GL_ARRAY_BUFFER, m_portalVbos[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(portal), portal, GL_STATIC_DRAW);
	}*/

	glBindVertexArray(0);

	return true;
}

void Renderer::resize(glm::uvec2 size)
{
	
}

void Renderer::upload(GameWorld* gameWorld)
{
	auto positions = parallsToTriangles(gameWorld->m_paralls);
	glBindBuffer(GL_ARRAY_BUFFER, m_sceneVbos[0]);
	glBufferData(GL_ARRAY_BUFFER,
		positions.size() * sizeof(glm::vec3),
		positions.data(),
		GL_STATIC_DRAW);

	auto normals = generateTriangleNormals(positions);
	glBindBuffer(GL_ARRAY_BUFFER, m_sceneVbos[1]);
	glBufferData(GL_ARRAY_BUFFER,
		normals.size() * sizeof(glm::vec3),
		normals.data(),
		GL_STATIC_DRAW);

	m_verticesCount = positions.size();
}

void Renderer::render(Graphics* graphics, GameWorld* gameWorld)
{
	upload(gameWorld);

	glUseProgram(m_programId);
	glUniform3f(glGetUniformLocation(m_programId, "clippingPositionCameraspace"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(m_programId, "clippingNormalCameraspace"), 0, 0, 0);
	renderScene(graphics, gameWorld, *graphics->m_camera);

	renderRecursive(graphics, gameWorld, *graphics->m_camera, 1);
}

void Renderer::renderRecursive(Graphics* graphics, GameWorld* gameWorld, Camera camera,
	int max_recursions, int recursion,
	std::vector<Parall> parentPortals, std::vector<glm::mat4> parentMVPs)
{
	if (recursion >= max_recursions)
		return;

	auto portals = getVisiblePortals(graphics, gameWorld, camera);

	for (std::size_t i = 0; i < portals.size(); i += 2) {
		auto& p0 = portals[i    ];
		auto& p1 = portals[i + 1];

		auto portalCamera = teleportCamera(camera, p0, p1);
		auto relevantPortals = parentPortals;
		relevantPortals.push_back(p0);
		auto relevantMVPs = parentMVPs;
		relevantMVPs.push_back(camera.m_lense.matrix() * camera.matrix());

		glStencilMask(0xff);
		glClear(GL_STENCIL_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilOp(GL_ZERO, GL_INCR, GL_INCR);
		glStencilMask(0xff);

		renderPortals(graphics, gameWorld, camera, relevantPortals, relevantMVPs);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilFunc(GL_EQUAL, recursion + 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);

		//std::cout << camera << std::endl << portalCamera << std::endl << std::endl;
		const auto& clippingPortal = p1;
		auto MV = portalCamera.matrix();
		auto clippingPositionCameraspace = MV * glm::vec4(clippingPortal.pos, 1);
		auto clippingNormalCameraspace =
			glm::normalize(MV * glm::vec4(glm::normalize(glm::cross(clippingPortal.right, clippingPortal.up)), 0));

		glUseProgram(m_programId);
		glUniform3f(glGetUniformLocation(m_programId, "clippingPositionCameraspace"),
			clippingPositionCameraspace.x, clippingPositionCameraspace.y, clippingPositionCameraspace.z);
		glUniform3f(glGetUniformLocation(m_programId, "clippingNormalCameraspace"),
			clippingNormalCameraspace.x, clippingNormalCameraspace.y, clippingNormalCameraspace.z);

		renderScene(graphics, gameWorld, portalCamera);

		glDisable(GL_STENCIL_TEST);

		renderRecursive(graphics, gameWorld, teleportCamera(camera, p0, p1), max_recursions, recursion + 1,
			relevantPortals, relevantMVPs);
	}
}

void Renderer::renderPortals(Graphics* graphics, GameWorld* gameWorld, Camera camera,
	std::vector<Parall> portals, std::vector<glm::mat4> MVPs)
{
	assert(portals.size() == MVPs.size());

	auto triangles = parallsToTriangles(portals);
	glBindBuffer(GL_ARRAY_BUFFER, m_portalVbos[0]);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3), triangles.data(), GL_STATIC_DRAW);

	glUseProgram(m_simpleProgramId);

	glBindVertexArray(m_portalVao);
	for (std::size_t i = 0; i < portals.size(); i++) {
		glUniformMatrix4fv(glGetUniformLocation(m_simpleProgramId, "MVP"), 1, GL_FALSE, &MVPs[i][0][0]);
		glDrawArrays(GL_TRIANGLES, i * 6, 6);
	}
	glBindVertexArray(0);
}

void Renderer::renderScene(Graphics* graphics, GameWorld* gameWorld, Camera camera)
{
	glUseProgram(m_programId);

	glm::mat4 M = glm::mat4(1.f);
	glm::mat4 V = camera.matrix();
	glm::mat4 P = camera.m_lense.matrix();
	glm::mat4 MVP = P * V * M;

	glUniformMatrix4fv(glGetUniformLocation(m_programId, "M"), 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_programId, "V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_programId, "MVP"), 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(m_sceneVao);
	glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
	glBindVertexArray(0);
}

std::vector<Parall> Renderer::getVisiblePortals(Graphics* graphics, GameWorld* gameWorld, Camera camera)
{
	std::vector<Parall> portals;

	for (auto& [i0, i1] : gameWorld->m_portalLinks)
	{
		auto& p0 = gameWorld->m_portals[i0];
		auto& p1 = gameWorld->m_portals[i1];

		glm::vec3 dir = glm::normalize(glm::cross(p0.right, p0.up));
		bool isFrontFace = glm::dot(dir, p0.pos - camera.m_pos) < 0;

		if (isFrontFace) {
			/* intersection check not yet implemented
			auto positions = parallsToTriangles({p0});
			std::transform(std::begin(positions), std::end(positions), std::begin(positions), [&] (auto& p) {
				return MVP * glm::vec4(p, 1);
			});

			bool isInScreen = intersect(positions, {
				{-1, -1, 0},
				{ 1, -1, 0},
				{ 1,  1, 0},
				{-1, -1, 0},
				{ 1,  1, 0},
				{-1,  1, 0}
			});

			if (isInScreen) {
				;*/

				portals.push_back(p0);
				portals.push_back(p1);

			//}
		}
	}

	return portals;
}

#include <glm/gtx/vector_angle.hpp>
Camera Renderer::teleportCamera(Camera camera, Parall inPortal, Parall outPortal)
{
	auto portalNormal = glm::normalize(glm::cross(inPortal.right, inPortal.up));
	auto otherNormal = glm::normalize(glm::cross(outPortal.right, outPortal.up));

	glm::vec3 direction = inPortal.pos - camera.m_pos;
	float distance = glm::length(direction);
	direction = glm::normalize(direction);

	glm::vec3 axis;
	if (portalNormal == otherNormal || portalNormal == -otherNormal)
		axis = glm::normalize(inPortal.up);
	else
		axis = glm::normalize(glm::cross(portalNormal, otherNormal));

	/*
	auto angle = glm::acos(glm::dot(portalNormal, -otherNormal));
	if (glm::dot(inPortal.right, -otherNormal) > 0)
		angle = glm::pi<float>() * 2.f - angle;
	*/

	float angle = glm::orientedAngle(portalNormal, otherNormal, axis) - glm::pi<float>();

	camera.m_pos = outPortal.pos + outPortal.right;

	camera.m_pos += glm::rotate(
		-direction * distance,
		angle,
		axis);

	camera.setDir(glm::rotate(
		camera.dir(),
		angle,
		axis));

	return camera;
}
