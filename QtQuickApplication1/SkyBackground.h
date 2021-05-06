#pragma once
#include "GLBackground.h"
#include "DirectionalLight.h"

class SkyBackground: public GLBackground
{
public:
	std::shared_ptr<DirectionalLight> light;
	Texture t0;
	Texture t1;
	SkyBackground(QOpenGLFunctions* _functions,
		const std::string& vertex = "Shaders/background.vs", const std::string& fragment = "Shaders/skyBackground.fs", const std::string& geometry = "Shaders/background.gs") :
		GLBackground(_functions, vertex, fragment, geometry)
	{
		t0 = Texture("Assets\\Sprites\\1.png");
		t0.setWrapMode(Texture::WrapMode::Repeat);
		t1 = Texture("Assets\\Sprites\\1.jpg");
		t1.setWrapMode(Texture::WrapMode::Repeat);

	}

	void render(std::shared_ptr<GLCamera>& camera) override
	{
		shader->bind();

		uploadCamera(camera);
		shader->setUniformValue("sunDir", light->direction);

		functions->glActiveTexture(GL_TEXTURE0 + 0);
		t0.texture->bind();
		shader->setUniformValue("iChannel0", 0);

		functions->glActiveTexture(GL_TEXTURE0 + 1);
		t1.texture->bind();
		shader->setUniformValue("iChannel1", 1);

		vao->bind();
		glDrawArrays(GL_POINTS, 0, 1);

	}
};
