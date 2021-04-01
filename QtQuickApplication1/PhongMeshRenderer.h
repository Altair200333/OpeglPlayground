#pragma once
#include "MeshRenderer.h"
#include <SpotLight.h>

#include "DirectionalLight.h"

class PhongMeshRenderer final: public MeshRenderer
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
	void bindShadowMap(unsigned int shadowMap)
	{
		if (shadowMap != -1)
		{
			functions->glActiveTexture(GL_TEXTURE4);
			functions->glBindTexture(GL_TEXTURE_2D, shadowMap);
			shader->setUniformValue("shadowMap", 4);
		}
	}
	void render(const RenderContext& context) override
	{
		if(!enabled)
			return;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader->bind();
		material->uploadToShader(shader);
		uploadCameraDetails(context.camera, shader);
		shader->setUniformValue(shader->uniformLocation("model"), transform->getGlobalTransform());

		uploadLights(context.lights);

		bindAlbedo();
		bindNormals();
		bindSpecular();
		
		if(context.lightSpaceMatrix != nullptr)
		{
			bindShadowMap(context.shadowMap);
			shader->setUniformValue("lightSpaceMatrix", *context.lightSpaceMatrix);
		}
		
		
		auto imgBack = std::dynamic_pointer_cast<ImageBackground>(context.background);
		if(imgBack !=nullptr)
		{
			shader->setUniformValue("texture_background", 3);
			functions->glActiveTexture(GL_TEXTURE3);
			imgBack->image->bind();
		}
		
		shader->setUniformValue("useBackground", context.background != nullptr);
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
