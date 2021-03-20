#pragma once
#include <string>
#include "MeshRendererGenerator.h"
#include <QOpenGLShaderProgram>

struct ShaderData final
{
	std::string vertex;
	std::string fragment;
	std::string geometry;

	std::shared_ptr<MeshRendererGenerator> renderer;
	std::shared_ptr<QOpenGLShaderProgram> shader = nullptr;
	
	ShaderData() = default;

	auto getShader()
	{
		if(shader == nullptr)
		{
			shader = std::make_shared<QOpenGLShaderProgram>();
			shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str());
			shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str());

			if (!geometry.empty())
				shader->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry.c_str());
			shader->link();
		}
		return shader;
	}
};
