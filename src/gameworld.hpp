#ifndef GAMEWORLD_HPP
#define GAMEWORLD_HPP

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "shapes.hpp"

using PortalPairs = std::map<std::size_t, std::size_t>;

class GameWorld {
public:
	GameWorld();

	std::vector<Parall> m_paralls;
	std::vector<Parall> m_portals;
	PortalPairs  m_portalLinks;
};

#endif
