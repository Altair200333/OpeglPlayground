#pragma once
#include <vector>


#include "Component.h"
#include "Vertex.h"

class Mesh final: public Component
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        this->vertices = vertices;
        this->indices = indices;
    }
};
