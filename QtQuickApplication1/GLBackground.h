#pragma once
#include <memory>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "GLcamera.h"

class GLBackground
{
public:
	std::shared_ptr<QOpenGLShaderProgram> shader = nullptr;
	QOpenGLVertexArrayObject* vao = nullptr;
	QOpenGLFunctions* functions;

	GLBackground() = default;
	GLBackground(QOpenGLFunctions * _functions,
		const std::string & vertex, const std::string & fragment, const std::string & geometry)
	{
		functions = _functions;
		shader = std::make_shared<QOpenGLShaderProgram>();

		shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str());
		shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str());
		shader->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry.c_str());

		shader->link();
		auto s = shader->log();

		vao = new QOpenGLVertexArrayObject();
		vao->create();
		vao->bind();

		vao->release();
		shader->release();
	}
	void uploadCamera(std::shared_ptr<GLCamera>& camera)
	{
		shader->setUniformValue("origin", camera->getPosition());
		shader->setUniformValue("front", camera->getForward());
		shader->setUniformValue("right", camera->getRight());
		shader->setUniformValue("up", camera->getUp());
		shader->setUniformValue("fov", camera->FOV);
		shader->setUniformValue("aspectRatio", camera->aspectRatio);
	}
	virtual void render(std::shared_ptr<GLCamera>& camera) = 0;
};
