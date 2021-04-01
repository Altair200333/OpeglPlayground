#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>



#include "GLBackground.h"
#include "GLCamera.h"

class ImageBackground final: public GLBackground
{
public:
	QOpenGLTexture* image = nullptr;
	
	ImageBackground() = default;
	ImageBackground(QOpenGLFunctions* _functions, const std::string& imagePath = "Assets\\Models\\textures\\background.jpg",
		const std::string& vertex = "Shaders/background.vs", const std::string& fragment = "Shaders/background.fs", const std::string& geometry = "Shaders/background.gs"):
		GLBackground(_functions, vertex, fragment, geometry)
	{
		image = new QOpenGLTexture(QImage(QString(imagePath.c_str())));
		image->setWrapMode(QOpenGLTexture::ClampToEdge);
	}



	void render(std::shared_ptr<GLCamera>& camera) override
	{
		shader->bind();

		shader->setUniformValue("background", 0);
		functions->glActiveTexture(GL_TEXTURE0 + 0);
		image->bind();

		uploadCamera(camera);

		shader->setUniformValue("width", image->width());
		shader->setUniformValue("height", image->height());
		
		vao->bind();

		
		glDrawArrays(GL_POINTS, 0, 1);

	}
};
