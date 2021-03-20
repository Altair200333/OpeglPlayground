#pragma once
#include <array>

struct Vertex
{
	std::array<float, 3> vertex;
	std::array<float, 3> normal;
	std::array<float, 3> color;
	std::array<float, 2> TexCoords;
	// tangent
	std::array<float, 3> Tangent;
	// bitangent
	std::array<float, 3> Bitangent;
};
