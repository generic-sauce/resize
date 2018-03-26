#include "gameworld.hpp"

using namespace glm;

GameWorld::GameWorld()
	: m_paralls{
		{vec3{ 0,  0,  0}, vec3{ 0,  0, 10}, vec3{10,  0,  0}}, // bottom
		{vec3{ 0, 10,  0}, vec3{10,  0,  0}, vec3{ 0,  0, 10}}, // top
		{vec3{ 0,  0,  0}, vec3{ 0, 10,  0}, vec3{ 0,  0, 10}}, // left
		{vec3{10,  0,  0}, vec3{ 0,  0, 10}, vec3{ 0, 10,  0}}, // right
		{vec3( 6,  6, 12), vec3( 2,  0,  0), vec3( 0,  2,  0)}, // something
		

		{vec3{20,  0,  0}, vec3{ 0,  0, 10}, vec3{10,  0,  0}}, // bottom
		{vec3{20, 10,  0}, vec3{10,  0,  0}, vec3{ 0,  0, 10}}, // top
		{vec3{20,  0,  0}, vec3{ 0, 10,  0}, vec3{ 0,  0, 10}}, // left
		{vec3{30,  0,  0}, vec3{ 0,  0, 10}, vec3{ 0, 10,  0}}, // right
		{vec3{-20, 0,  0}, 12.f * glm::normalize(vec3{ 1,  0,  1}), vec3{0,  1,  0}}, // bottom
		{vec3{-20,11,  0}, 12.f * glm::normalize(vec3{ 1,  0,  1}), vec3{0,  1,  0}}, // top
		{vec3{-20, 1,  0},        glm::normalize(vec3{ 1,  0,  1}), vec3{0, 10,  0}}, // left
		{vec3{-20, 1,  0}+ 11.f * glm::normalize(vec3( 1,  0,  1))
		                 ,        glm::normalize(vec3{ 1,  0,  1}), vec3{0, 10,  0}}, // right
	}
	, m_portals{
		{vec3( 0,  0,  0), vec3( 10,  0,  0), vec3( 0, 10,  0)}, // back
		{vec3(10,  0, 10), vec3(-10,  0,  0), vec3( 0, 10,  0)}, // front

		{vec3(20,  0,  0), vec3( 10,  0,  0), vec3( 0, 10,  0)},
		{vec3(-20, 1,  0) + glm::normalize(glm::vec3( 1,  0,  1)),
			10.f * glm::normalize(vec3( 1,  0,  1)),
			vec3( 0, 10,  0)} // center
	}
	, m_portalLinks{
		{
			{0, 1},
			{1, 0},
			{2, 3},
			{3, 2},
		}}
{}
