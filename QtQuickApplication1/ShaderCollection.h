#pragma once
#include <map>

#include "ShaderData.h"
#include "PhongMeshRenderer.h"
#include "VolumetricCubeMeshRenderer.h"

class ShaderCollection final
{

	class SimpleMeshRendererGenerator final : public MeshRendererGenerator
	{
	public:
		std::shared_ptr<MeshRenderer> getRenderer() override
		{
			return std::make_shared<PhongMeshRenderer>();
		}
	};
	class VolumetricCubeMeshRendererGenerator final : public MeshRendererGenerator
	{
	public:
		std::shared_ptr<MeshRenderer> getRenderer() override
		{
			return std::make_shared<VolumetricCubeMeshRenderer>();
		}
	};

public:
	inline static std::map<std::string, ShaderData> shaders =
	{
		{{"normals"}, {"Shaders\\normals.vs","Shaders\\normals.fs", "", std::make_shared<SimpleMeshRendererGenerator>()}},
		{{"pick"}, {"Shaders\\pick.vs","Shaders\\pick.fs", "", std::make_shared<SimpleMeshRendererGenerator>()}},
		{{"lightSource"}, {"Shaders\\lightSource.vs","Shaders\\lightSource.fs", "", std::make_shared<SimpleMeshRendererGenerator>()}},
		{{"cubicCloud"}, {"Shaders\\cloud.vs","Shaders\\cloud.fs", "", std::make_shared<VolumetricCubeMeshRendererGenerator>()}},
		{{"sprite"}, {"Shaders\\sprite.vs","Shaders\\sprite.fs", "Shaders\\sprite.gs"}},
	};
};
