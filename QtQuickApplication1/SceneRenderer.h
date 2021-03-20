#pragma once
#include "Scene.h"

class SceneRenderer final
{
public:
	int drawMode = 0;

	void nextDrawMode()
	{
		drawMode = drawMode == 2 ? 0 : drawMode + 1;
	}
	void renderLights(Scene& scene)
	{
		for (auto& light : scene.lights)
		{
			scene.lightSourceBlock->transform->translate(-scene.lightSourceBlock->transform->position);
			scene.lightSourceBlock->transform->translate(light->position);
			scene.lightSourceBlock->material->diffuse = light->color;
			scene.lightSourceBlock->material->isLightSource = true;
			scene.lightSourceBlock->renderer->render(scene.camera, scene.lights);
		}
	}

	void renderTransparentObjects(Scene& scene)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		for (auto& cloud : scene.transparentObjects)
			cloud->renderer->render(scene.camera, scene.lights);
		glCullFace(GL_FRONT);
		for (auto& cloud : scene.transparentObjects)
			cloud->renderer->render(scene.camera, scene.lights);
	}

	void render(Scene& scene)
	{
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		for (size_t i = 0; i < scene.objects.size(); ++i)
		{
			if(drawMode == 0)
				scene.objects[i]->renderer->render(scene.camera, scene.lights, scene.backround);
			else if(drawMode == 1)
			{
				scene.objects[i]->renderer->render(scene.camera, scene.lights, scene.backround);

				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1, -1);

				scene.objects[i]->renderer->renderWireframe(scene.camera);

				glDisable(GL_POLYGON_OFFSET_LINE);
			}
			else
			{
				scene.objects[i]->renderer->renderWireframe(scene.camera);
			}
		}
		
		renderLights(scene);
		scene.backround->render(scene.camera);

		renderTransparentObjects(scene);
	}
};