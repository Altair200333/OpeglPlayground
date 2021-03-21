#pragma once
#include "MeshRenderer.h"
#include <SpotLight.h>

#include "DirectionalLight.h"

class SimpleMeshRenderer final: public MeshRenderer
{
public:

	void bindAlbedo()
	{
		shader->setUniformValue("albedoCount", static_cast<int>(material->textures.size()));
		if(!material->textures.empty())
		{
			functions->glActiveTexture(GL_TEXTURE0 + 0);
			material->textures[0].texture->bind();
			shader->setUniformValue("texture_diffuse", 0);
		}
	}
	void bindNormals()
	{
		shader->setUniformValue("normalCount", static_cast<int>(material->normal.size()));
		if (!material->normal.empty())
		{
			functions->glActiveTexture(GL_TEXTURE1);
			material->normal[0].texture->bind();
			shader->setUniformValue("texture_normal", 1);
		}
	}
	void bindSpecular()
	{
		shader->setUniformValue("specularCount", static_cast<int>(material->specular.size()));
		if (!material->specular.empty())
		{
			functions->glActiveTexture(GL_TEXTURE2);
			material->specular[0].texture->bind();
			shader->setUniformValue("texture_specular", 2);
		}
	}
	void render(GLCamera& camera, const std::vector<std::shared_ptr<LightSource>>& lights = std::vector<std::shared_ptr<LightSource>>{}, 
		std::shared_ptr<Background> background = nullptr) override
	{
		if(!enabled)
			return;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader->bind();
		material->uploadToShader(shader);
		uploadCameraDetails(camera);
		uploadLights(lights);

		bindAlbedo();
		bindNormals();
		bindSpecular();
		
		if(background!=nullptr)
		{
			shader->setUniformValue("texture_background", 3);
			functions->glActiveTexture(GL_TEXTURE0 + 3);
			background->image->bind();
		}
		
		shader->setUniformValue("useBackground", background != nullptr);
		shader->setUniformValue("isLightSource", material->isLightSource);

		shader->setUniformValue("wireframe", false);
		vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		vao->release();

		functions->glActiveTexture(GL_TEXTURE0);
	}
	void uploadLights(const std::vector<std::shared_ptr<LightSource>>& lights) const
	{
		size_t pointLights = 0;
		size_t spotLights = 0;
		size_t dirLights = 0;
		for (int i = 0; i < lights.size(); ++i)
		{	
			int id = 0;
			if (std::dynamic_pointer_cast<PointLight>(lights[i]) != nullptr)
			{
				id = pointLights++;
			}
			else if (std::dynamic_pointer_cast<SpotLight>(lights[i]) != nullptr)
			{
				id = spotLights++;
			}
			else if (std::dynamic_pointer_cast<DirectionalLight>(lights[i]) != nullptr)
			{
				id = dirLights++;
			}
			lights[i]->uploadToShader(shader, id);
		}
		
		shader->setUniformValue("lightsCount", static_cast<int>(pointLights));
		shader->setUniformValue("spotLightsCount", static_cast<int>(spotLights));
		shader->setUniformValue("dirLightsCount", static_cast<int>(dirLights));

	}
};
