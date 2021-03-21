#pragma once
#include "MeshRenderer.h"

class VolumetricCubeMeshRenderer final: public MeshRenderer
{
public:
	void render(GLCamera& camera, 
		const std::vector<std::shared_ptr<LightSource>>& lights = std::vector<std::shared_ptr<LightSource>>{}, std::shared_ptr<Background> background = nullptr) override
	{
		std::shared_ptr<Mesh> mesh = ComponentManager::getComponent<Mesh>(owner);
		std::shared_ptr<Material> material = ComponentManager::getComponent<Material>(owner);
		std::shared_ptr<Transform> transform = ComponentManager::getComponent<Transform>(owner);

		if(!enabled)
			return;
		shader->bind();

		material->uploadToShader(shader);
		uploadCameraDetails(camera);
		shader->setUniformValue("position", transform->position);
		
		vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		vao->release();
	}
};