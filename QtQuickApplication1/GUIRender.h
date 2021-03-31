#pragma once
#include "GLWindow.h"
#include "RenderLayer.h"
#include "Scene.h"

class GUIRenderer final: public RenderLayer
{
protected:
	void onRender() override
	{
		for(auto& sprite: scene->sprites)
		{
			sprite->viewportW = viewport->width();
			sprite->viewportH = viewport->height();
			renderSprite(sprite);
		}
	}
	void renderSprite(std::shared_ptr<Sprite>& sprite)
	{
		glDisable(GL_DEPTH_TEST);

		sprite->vao->bind();
		auto shader = ShaderCollection::shaders["sprite"].getShader();
		shader->bind();
	
		shader->setUniformValue("w", sprite->width);
		shader->setUniformValue("h", sprite->height);

		shader->setUniformValue("viewportW", sprite->viewportW);
		shader->setUniformValue("viewportH", sprite->viewportH);

		viewport->glActiveTexture(GL_TEXTURE0 + 0);
		sprite->texture->bind();
		shader->setUniformValue("sprite", 0);

		glDrawArrays(GL_POINTS, 0, 1);

		glEnable(GL_DEPTH_TEST);
	}
public:
	std::shared_ptr<Scene> scene;
	std::shared_ptr<GLWindow> viewport;
};
