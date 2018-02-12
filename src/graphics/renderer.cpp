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

std::ostream& operator<< (std::ostream& os, const glm::vec3& v)
{
	return os << "(" << v.x << " | " << v.y << " | " << v.z << ")";
}

template <typename InIt, typename OutContainer>
inline void generateTriangleNormals(InIt begin, InIt end, std::back_insert_iterator<OutContainer> outIt)
{
	for (auto it = begin; it + 2 < end; it += 3) {
		const auto& p0 = *it;
		const auto& p1 = *(it + 1);
		const auto& p2 = *(it + 2);
		auto normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
		*outIt = normal;
		*outIt = normal;
		*outIt = normal;
	}
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

	m_textureProgramId = loadProgram({
		{GL_VERTEX_SHADER, "data/shader/texture_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "data/shader/texture_fragment.glsl"},
		});

	if (!m_textureProgramId)
		return false;

	m_simpleProgramId = loadProgram({
		{GL_VERTEX_SHADER, "data/shader/simple_vertex.glsl"},
		{GL_FRAGMENT_SHADER, "data/shader/simple_fragment.glsl"},
		});

	if (!m_simpleProgramId)
		return false;

	glCreateVertexArrays(1, &m_vao);
	glGenBuffers(2, &m_vbos[0]);

	glBindVertexArray(m_vao);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		nullptr);

	glBindVertexArray(0);

	return true;
}

void Renderer::resize(glm::uvec2 size)
{
}

void Renderer::render(Graphics *graphics, GameWorld* world)
{
	std::cout << "-- render --" << std::endl;

	float plainDepthTexture [graphics->windowSize().x * graphics->windowSize().y];
	for (std::size_t i = 0; i < graphics->windowSize().x * graphics->windowSize().y; i++)
		plainDepthTexture[i] = (std::rand() % 1000) / 1000.f;
	
	static float recursions = 1.f;
	if (glfwGetKey(graphics->window(), GLFW_KEY_T))
		recursions += 0.2f;
	if (glfwGetKey(graphics->window(), GLFW_KEY_G))
		recursions -= 0.2f;
	recursions = std::max(0.f, recursions);

	glm::vec3 clippingPositionCameraspace(0, 0, 0);
	glUniform3f(glGetUniformLocation(m_programId, "clippingPositionCameraspace"),
		clippingPositionCameraspace.x,
		clippingPositionCameraspace.y,
		clippingPositionCameraspace.z);
	glm::vec3 clippingNormalCameraspace(0, 0, 1);
	glUniform3f(glGetUniformLocation(m_programId, "clippingNormalCameraspace"),
		clippingNormalCameraspace.x,
		clippingNormalCameraspace.y,
		clippingNormalCameraspace.z);

	std::size_t count = prepareRender(graphics, world);
	renderWorld(graphics, count, *graphics->m_camera);
	renderPortals(graphics, world, count, *graphics->m_camera, recursions);
}

std::size_t Renderer::prepareRender(Graphics* graphics, GameWorld* world)
{
	std::vector<glm::vec3> vertices;
	vertices.reserve(world->m_paralls.size() * 6);
	for (auto& rect : world->m_paralls) {
		vertices.push_back(rect.pos);
		vertices.push_back(rect.pos + rect.right);
		vertices.push_back(rect.pos + rect.right + rect.up);
		vertices.push_back(rect.pos);
		vertices.push_back(rect.pos + rect.up + rect.right);
		vertices.push_back(rect.pos + rect.up);
	}

	std::vector<glm::vec3> normals;
	normals.reserve(vertices.size());
	generateTriangleNormals(std::begin(vertices), std::end(vertices), std::back_inserter(normals));

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			vertices.data(),
			GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER,
			normals.size() * sizeof(glm::vec3),
			normals.data(),
			GL_STATIC_DRAW);

	return vertices.size();
}

void Renderer::renderWorld(Graphics* graphics, std::size_t count, Camera camera)
{
	std::cout << "render" << std::endl;

	//glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programId);

	glm::mat4 M = glm::mat4(1.f);
	glm::mat4 V = camera.matrix();
	glm::mat4 P = camera.m_lense.matrix();
	glm::mat4 MVP = P * V * M;

	glUniformMatrix4fv(glGetUniformLocation(m_programId, "M"), 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_programId, "V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_programId, "MVP"), 1, GL_FALSE, &MVP[0][0]);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, count);
	glBindVertexArray(0);
}

void Renderer::renderPortals(Graphics* graphics, GameWorld* world, std::size_t count, Camera renderCamera, std::size_t recursions)
{
	if (recursions <= 0)
		return;

	glm::mat4 M = glm::mat4(1.f);
	glm::mat4 V = renderCamera.matrix();
	glm::mat4 MV = V * M;

	for (std::size_t i = 0; i < world->m_portals.size(); i++)
	{
		const auto& portal = world->m_portals[i];
		Camera camera = renderCamera;

		auto it = world->m_portalLinks.find(i);
		//assert(it != world->m_portalLinks.end());
		if (it == world->m_portalLinks.end())
			continue;

		const auto& other = world->m_portals[it->second];

		auto portalNormal = glm::normalize(glm::cross(portal.right, portal.up));
		auto otherNormal = glm::normalize(glm::cross(other.right, other.up));

		glm::vec3 direction = portal.pos - camera.m_pos;
		float distance = glm::length(direction);
		direction = glm::normalize(direction);

		float dot = glm::dot(portalNormal, -direction);
		if (dot <= 0) {
			std::cout << "skipping portal" << std::endl;
			continue;
		}
		{ // TODO intersect screen
		}

		std::cout << "portal" << std::endl;

		auto axis = glm::normalize(glm::cross(portalNormal, otherNormal + glm::vec3(1, 1, 1) * 0.001f));

		auto angle = glm::acos(glm::dot(portalNormal, -otherNormal));
		if (glm::dot(portal.right, -otherNormal) > 0)
			angle = glm::pi<float>() * 2.f - angle;

		camera.m_pos = other.pos + glm::rotate(
			-direction * distance,
			angle,
			axis);

		camera.setDir(glm::rotate(
			camera.dir(),
			angle,
			axis));

		//std::cout << renderCamera.m_pos << renderCamera.dir() << std::endl;
		//std::cout << camera.m_pos << camera.dir() << std::endl;

		glm::vec3 clippingPositionCameraspace = MV * glm::vec4(portal.pos, 1);
		glUniform3f(glGetUniformLocation(m_programId, "clippingPositionCameraspace"),
			clippingPositionCameraspace.x,
			clippingPositionCameraspace.y,
			clippingPositionCameraspace.z);
		glm::vec3 clippingNormalCameraspace = MV * glm::vec4(portalNormal, 0);
		//std::cout << portalNormal << clippingPositionCameraspace << std::endl;
		glUniform3f(glGetUniformLocation(m_programId, "clippingNormalCameraspace"),
			clippingNormalCameraspace.x,
			clippingNormalCameraspace.y,
			clippingNormalCameraspace.z);

		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		glClear(GL_STENCIL_BUFFER_BIT);

		renderPortalFace(graphics, portal, renderCamera);

		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		renderWorld(graphics, prepareRender(graphics, world), camera);

		glDisable(GL_STENCIL_TEST);

		renderPortals(graphics, world, count, camera, recursions - 1);
	}
}

void Renderer::renderPortalFace(Graphics* graphics, const Parall& portal, Camera& camera)
{
	glm::vec3 vertices[] {
		portal.pos,
		portal.pos + portal.right,
		portal.pos + portal.right + portal.up,
		portal.pos,
		portal.pos + portal.right + portal.up,
		portal.pos + portal.up,
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glm::mat4 MVP = camera.m_lense.matrix() * camera.matrix() * glm::mat4(1.f);
	glUniformMatrix4fv(glGetUniformLocation(m_programId, "MVP"), 1, GL_FALSE, &MVP[0][0]);

	//glUseProgram(m_simpleProgramId);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::render(Graphics* graphics, GLuint textureId)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_textureProgramId);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_textureProgramId, "tex"), 0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	using namespace glm;
	vec3 vertices[] = {
		vec3(-1, -1, 0),
		vec3( 1, -1, 0),
		vec3( 1,  1, 0),

		vec3(-1, -1, 0),
		vec3( 1,  1, 0),
		vec3(-1,  1, 0)
	};

	vec3 uvs[] = {
		vec3(0, 0, 0),
		vec3(1, 0, 0),
		vec3(1, 1, 0),

		vec3(0, 0, 0),
		vec3(1, 1, 0),
		vec3(0, 1, 0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}
