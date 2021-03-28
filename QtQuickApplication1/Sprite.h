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

	Sprite(const std::string& path, int w, int h):width(w), height(h)
	{
		vao = new QOpenGLVertexArrayObject();
		vao->create();
		vao->bind();

		texture = std::make_shared<QOpenGLTexture>(QImage(QString(path.c_str())));
	}
	
};
