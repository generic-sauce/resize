#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <glm/glm.hpp>

class Graphics;
class GameWorld;
class Camera;

class Renderer {
public:
	Renderer();
	~Renderer();

	bool init(Graphics* graphics);
	void resize(glm::uvec2 size);
	void render(Graphics* graphics, GameWorld* world);

private:
	GLuint m_programId;
	GLuint m_simpleProgramId;
	GLuint m_textureProgramId;

	GLuint m_vao;
	GLuint m_vbos[2];

	std::size_t prepareRender(Graphics* graphics, GameWorld* world);
	void renderWorld(Graphics* graphics, std::size_t count, Camera camera);
	void renderPortals(Graphics* graphics, GameWorld* world, std::size_t count, Camera renderCamera, std::size_t recursions);
	void renderPortalFace(Graphics* graphics, const class Parall& portal, Camera& camera);
	void render(Graphics* graphics, GLuint textureId);
};

#endif
