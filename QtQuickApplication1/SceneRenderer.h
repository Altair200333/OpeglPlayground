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
	std::unordered_map<std::shared_ptr<Object>, bool> initializedPickShaders;
	
	void renderPickShader(std::shared_ptr<Scene>& scene)
	{
		auto shader = ShaderCollection::shaders["pick"].getShader();
		shader->bind();

		for (int i = 0; i < scene->objects.size(); ++i)
		{
			int r = (i & 0x000000FF) >> 0;
			int g = (i & 0x0000FF00) >> 8;
			int b = (i & 0x00FF0000) >> 16;
			
			shader->setUniformValue("color", QColor(r,g,b));
			auto meshRenderer = ComponentManager::getComponent<MeshRenderer>(scene->objects[i]);
			const auto mesh = ComponentManager::getComponent<Mesh>(scene->objects[i]);
			const auto transform = ComponentManager::getComponent<Transform>(scene->objects[i]);

			if(!initializedPickShaders[scene->objects[i]])
			{
				meshRenderer->enableAttributes(shader);
				initializedPickShaders[scene->objects[i]] = true;
			}
			
			meshRenderer->uploadCameraDetails(scene->camera, shader);
			shader->setUniformValue(shader->uniformLocation("model"), transform->getGlobalTransform());

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			meshRenderer->vao->bind();
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			meshRenderer->vao->release();
		}
	}

	void render(std::shared_ptr<Scene>& scene)
	{
		//Enable blending
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
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);
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
