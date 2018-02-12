#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "shapes.hpp"

class GameWorld {
public:
	GameWorld();

	std::vector<Parall> m_paralls;
	std::vector<Parall> m_portals;
	std::map<std::size_t, std::size_t> m_portalLinks;
};

#endif
