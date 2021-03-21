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

	void renderLights(std::shared_ptr<Scene>& scene)
	{
		for (auto& light : scene->lights)
		{
			auto transform = ComponentManager::getComponent<Transform>(scene->lightSourceBlock);
			transform->translate(-transform->position);
			transform->translate(light->position);

			ComponentManager::getComponent<Material>(scene->lightSourceBlock)->diffuse = light->color;
			ComponentManager::getComponent<Material>(scene->lightSourceBlock)->isLightSource = true;
			ComponentManager::getComponent<MeshRenderer>(scene->lightSourceBlock)->render(scene->camera, scene->lights);
		}
	}

	void renderTransparentObjects(std::shared_ptr<Scene>& scene)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render(scene->camera, scene->lights);
		glCullFace(GL_FRONT);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render(scene->camera, scene->lights);
		glCullFace(GL_BACK);
	}

	void renderPickShader(std::shared_ptr<Scene>& scene)
	{
		auto shader = ShaderCollection::shaders["pick"].getShader();
		shader->bind();

		for (int i = 0; i < scene->objects.size(); ++i)
		{
			shader->setUniformValue("id", i);
			auto meshRenderer = ComponentManager::getComponent<MeshRenderer>(scene->objects[i]);
			auto mesh = ComponentManager::getComponent<Mesh>(scene->objects[i]);
			meshRenderer->enableAttributes(shader);
			meshRenderer->uploadCameraDetails(scene->camera, shader);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			meshRenderer->vao->bind();
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			meshRenderer->vao->release();
		}
	}

	void render(std::shared_ptr<Scene>& scene)
	{
		//enderPickShader(scene);
		//lClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//return;
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		for (size_t i = 0; i < scene->objects.size(); ++i)
		{
			if (drawMode == 0)
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render(
					scene->camera, scene->lights, scene->backround);
			else if (drawMode == 1)
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render(
					scene->camera, scene->lights, scene->backround);

				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1, -1);

				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);

				glDisable(GL_POLYGON_OFFSET_LINE);
			}
			else
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);
			}
		}

		renderLights(scene);
		scene->backround->render(scene->camera);

		renderTransparentObjects(scene);

	}
};
