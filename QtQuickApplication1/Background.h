#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>


#include "GLCamera.h"

class Background final
{
public:
	std::shared_ptr<QOpenGLShaderProgram> shader = nullptr;
	QOpenGLVertexArrayObject* vao = nullptr;
	QOpenGLTexture* image = nullptr;
	QOpenGLFunctions* functions;
	
	Background() = default;
	Background(QOpenGLFunctions* _functions, const std::string& imagePath = "Assets\\Models\\textures\\background.jpg",
		const std::string& vertex = "Shaders/background.vs", const std::string& fragment = "Shaders/background.fs", const std::string& geometry = "Shaders/background.gs")
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

		image = new QOpenGLTexture(QImage(QString(imagePath.c_str())));
	}
	
	void render(GLCamera& camera)
	{
		shader->bind();

		shader->setUniformValue("background", 0);
		functions->glActiveTexture(GL_TEXTURE0 + 0);
		image->bind();

		shader->setUniformValue("origin", camera.position);
		shader->setUniformValue("front", camera.front);
		shader->setUniformValue("right", camera.right);
		shader->setUniformValue("up", camera.up);
		shader->setUniformValue("fov", camera.FOV);
		shader->setUniformValue("aspectRatio", camera.aspectRatio);

		shader->setUniformValue("width", image->width());
		shader->setUniformValue("height", image->height());
		
		vao->bind();

		
		glDrawArrays(GL_POINTS, 0, 1);

	}
};
