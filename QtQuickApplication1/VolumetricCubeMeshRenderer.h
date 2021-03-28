#pragma once
#include "MeshRenderer.h"

class VolumetricCubeMeshRenderer final: public MeshRenderer
{
public:
	void render(GLCamera& camera, 
		const std::vector<std::shared_ptr<LightSource>>& lights = std::vector<std::shared_ptr<LightSource>>{}, std::shared_ptr<GLBackground> background = nullptr,
		unsigned int shadowMap = -1, QMatrix4x4* lightSpaceMatrix = nullptr) override
	{
		std::shared_ptr<Mesh> mesh = ComponentManager::getComponent<Mesh>(owner);
		std::shared_ptr<Material> material = ComponentManager::getComponent<Material>(owner);
		std::shared_ptr<Transform> transform = ComponentManager::getComponent<Transform>(owner);

		if(!enabled)
			return;
		shader->bind();

		material->uploadToShader(shader);
		uploadCameraDetails(camera, shader);
		shader->setUniformValue("position", transform->position);
		
		vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		vao->release();
	}
};