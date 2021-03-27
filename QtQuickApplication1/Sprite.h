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

	Sprite(int w, int h):width(w), height(h)
	{
		vao = new QOpenGLVertexArrayObject();
		vao->create();
		vao->bind();
	}
	void render()
	{
		glDisable(GL_DEPTH_TEST);

		vao->bind();
		auto shader = ShaderCollection::shaders["sprite"].getShader();
		shader->bind();
		shader->setUniformValue("w", width);
		shader->setUniformValue("h", height);

		shader->setUniformValue("viewportW", viewportW);
		shader->setUniformValue("viewportH", viewportH);
		
		glDrawArrays(GL_POINTS, 0, 1);

		glEnable(GL_DEPTH_TEST);
	}
};
