#pragma once
#include "GLBackground.h"
#include "DirectionalLight.h"

class SkyBackground: public GLBackground
{
public:
	std::shared_ptr<DirectionalLight> light;
	
	SkyBackground(QOpenGLFunctions* _functions,
		const std::string& vertex = "Shaders/background.vs", const std::string& fragment = "Shaders/skyBackground.fs", const std::string& geometry = "Shaders/background.gs") :
		GLBackground(_functions, vertex, fragment, geometry)
	{
		
	}

	void render(GLCamera& camera) override
	{
		shader->bind();

		uploadCamera(camera);
		shader->setUniformValue("sunDir", light->direction);
		vao->bind();
		glDrawArrays(GL_POINTS, 0, 1);

	}
};
