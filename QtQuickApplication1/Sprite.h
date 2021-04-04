#pragma once
#include <memory>
#include <QOpenGLTexture>

#include "ShaderCollection.h"

class Sprite final
{
public:
	std::shared_ptr<QOpenGLTexture> texture;
	int width, height;
	int x=0, y=0;
	int viewportW=400, viewportH=400;
	
	QOpenGLVertexArrayObject* vao = nullptr;

	Sprite(const std::string& path, int w, int h, int _x =0, int _y=0):width(w), height(h), x(_x), y(_y)
	{
		vao = new QOpenGLVertexArrayObject();
		vao->create();
		vao->bind();

		texture = std::make_shared<QOpenGLTexture>(QImage(QString(path.c_str())));
		texture->bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		float aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		texture->generateMipMaps();
	}
	
};
