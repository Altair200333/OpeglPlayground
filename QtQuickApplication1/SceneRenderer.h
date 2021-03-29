#pragma once
#include "AloyApplication.h"
#include "RenderLayer.h"
#include "Scene.h"

class SceneRenderer final: public RenderLayer
{
public:
	int drawMode = 0;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<GLWindow> viewport;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

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
			ComponentManager::getComponent<MeshRenderer>(scene->lightSourceBlock)->render({ &scene->camera, scene->lights });
		}
	}

	void renderTransparentObjects(std::shared_ptr<Scene>& scene)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render({ &scene->camera, scene->lights });
		glCullFace(GL_FRONT);
		for (auto& cloud : scene->transparentObjects)
			ComponentManager::getComponent<MeshRenderer>(cloud)->render({ &scene->camera, scene->lights });
		glCullFace(GL_BACK);
	}

	void renderObjects()
	{
		for (size_t i = 0; i < scene->objects.size(); ++i)
		{
			if (drawMode == 0)
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render({
					&scene->camera, scene->lights, scene->backround, depthMap, &lightSpaceMatrix });
			else if (drawMode == 1)
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->render(
					{&scene->camera, scene->lights, scene->backround, depthMap, &lightSpaceMatrix });
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);
			}
			else
			{
				ComponentManager::getComponent<MeshRenderer>(scene->objects[i])->renderWireframe(scene->camera);
			}
		}
	}

	void onRender() override
	{
		if (!initialized)
			initShadowMap();
		renderShadows();

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		renderObjects();

		renderLights(scene);
		scene->backround->render(scene->camera);

		renderTransparentObjects(scene);

	}
	
	unsigned int depthMapFBO;
	unsigned int depthMap;
	bool initialized = false;
	std::unordered_map<std::shared_ptr<Object>, bool> initializedShadowShaders;
	QMatrix4x4 lightSpaceMatrix;
	void renderShadows()
	{
		std::shared_ptr<DirectionalLight> dirLight;//TODO fix empty case
		for(auto& light: scene->lights)
		{
			auto ptr = std::dynamic_pointer_cast<DirectionalLight>(light);
			if (ptr != nullptr)
				dirLight = ptr;
		}
		const float near_plane = 0.1f, far_plane = 100.5f;

		QMatrix4x4 lightProjection;
		float size = 20;
		lightProjection.ortho(-size, size, -size, size, near_plane, far_plane);

		QMatrix4x4 lightView;
		lightView.lookAt( scene->camera.position-dirLight->direction*10, scene->camera.position + dirLight->direction * 3, QVector3D(0, 1, 0));

		lightSpaceMatrix = lightProjection* lightView;
		
		viewport->glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		viewport->glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		auto shader = ShaderCollection::shaders["shadow"].getShader();
		shader->bind();
		shader->setUniformValue("lightSpaceMatrix", lightSpaceMatrix);
		for (int i = 0; i < scene->objects.size(); ++i)
		{
			auto meshRenderer = ComponentManager::getComponent<MeshRenderer>(scene->objects[i]);
			const auto mesh = ComponentManager::getComponent<Mesh>(scene->objects[i]);
			const auto transform = ComponentManager::getComponent<Transform>(scene->objects[i]);

			if (!initializedShadowShaders[scene->objects[i]])
			{
				meshRenderer->enableAttributes(shader);
				initializedShadowShaders[scene->objects[i]] = true;
			}
			
			shader->setUniformValue(shader->uniformLocation("model"), transform->getGlobalTransform());
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			meshRenderer->vao->bind();
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			meshRenderer->vao->release();
		}
		viewport->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, viewport->width(), viewport->height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void initShadowMap()
	{
		viewport->glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		viewport->glGenTextures(1, &depthMap);
		viewport->glBindTexture(GL_TEXTURE_2D, depthMap);
		viewport->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		viewport->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		viewport->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		viewport->glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		viewport->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		viewport->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		initialized = true;
	}
};
