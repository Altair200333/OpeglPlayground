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
			auto transform = scene.lightSourceBlock->getComponent<Transform>();
			transform->translate(-transform->position);
			transform->translate(light->position);
			
			scene.lightSourceBlock->getComponent<Material>()->diffuse = light->color;
			scene.lightSourceBlock->getComponent<Material>()->isLightSource = true;
			scene.lightSourceBlock->getComponent<MeshRenderer>()->render(scene.camera, scene.lights);
		}
	}

	void renderTransparentObjects(Scene& scene)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		for (auto& cloud : scene.transparentObjects)
			cloud->getComponent<MeshRenderer>()->render(scene.camera, scene.lights);
		glCullFace(GL_FRONT);
		for (auto& cloud : scene.transparentObjects)
			cloud->getComponent<MeshRenderer>()->render(scene.camera, scene.lights);
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
				scene.objects[i]->getComponent<MeshRenderer>()->render(scene.camera, scene.lights, scene.backround);
			else if(drawMode == 1)
			{
				scene.objects[i]->getComponent<MeshRenderer>()->render(scene.camera, scene.lights, scene.backround);

				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1, -1);

				scene.objects[i]->getComponent<MeshRenderer>()->renderWireframe(scene.camera);

				glDisable(GL_POLYGON_OFFSET_LINE);
			}
			else
			{
				scene.objects[i]->getComponent<MeshRenderer>()->renderWireframe(scene.camera);
			}
		}
		
		renderLights(scene);
		scene.backround->render(scene.camera);

		renderTransparentObjects(scene);
	}
};