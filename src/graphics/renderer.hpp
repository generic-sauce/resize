#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <glm/glm.hpp>
#include <gameworld.hpp>

class Graphics;
class GameWorld;
class Camera;

class Renderer {
public:
	Renderer();
	~Renderer();

	bool init(Graphics* graphics);
	void resize(glm::uvec2 size);
	void upload(GameWorld* gameWorld);
	void render(Graphics* graphics, GameWorld* gameWorld);

private:
	void renderRecursive(Graphics* graphics, GameWorld* gameWorld, Camera camera, int max_recursions,
		int recursion = 0, std::vector<Parall> parentPortals = {}, std::vector<glm::mat4> parentMVPs = {});
	void renderPortals(Graphics* graphics, GameWorld* gameWorld,
		Camera camera, std::vector<Parall> portals, std::vector<glm::mat4> MVPs);
	void renderScene(Graphics* graphics, GameWorld* gameWorld, Camera camera);

	std::vector<Parall> getVisiblePortals(Graphics* graphics, GameWorld* gameWorld, Camera camera);
	Camera teleportCamera(Camera camera, Parall inPortal, Parall outPortal);

private:
	GLuint m_programId;
	GLuint m_simpleProgramId;
	GLuint m_textureProgramId;

	GLuint m_sceneVao;
	GLuint m_sceneVbos[2];
	GLuint m_portalVao;
	GLuint m_portalVbos[1];

	std::size_t m_verticesCount;
};

#endif
