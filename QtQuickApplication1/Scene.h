#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <QObject>

#include "Input.h"
#include "MouseInput.h"
#include "Object.h"
#include <QTransform>


#include "Background.h"
#include "MeshLoader.h"
#include "ShaderCollection.h"
#include "ShaderData.h"
#include "VolumetricCubeMeshRenderer.h"

class Scene
{
protected:
	void createLightSourceBlock()
	{
		lightSourceBlock = createObject(MeshLoader().loadModel("Assets/Models/ico.obj")[0], QVector3D(0, 0, 0),
			ShaderCollection::shaders["lightSource"]);
	}
	std::shared_ptr<Object> createObject(const MeshLoader::LoadedModel& model, const QVector3D& pos, ShaderData& data, const std::string& tag = "") const
	{
		auto object = std::make_shared<Object>();
		object->tag = tag;
		
		object->addComponent(std::make_shared<Transform>());
		object->addComponent(model.mesh);
		object->addComponent(model.material);
		object->addComponent(data.renderer->getRenderer());
		
		object->getComponent<Transform>()->translate(pos);
		object->getComponent<MeshRenderer>()->initMeshRenderer(functions, object->getComponent<Transform>(),
			object->getComponent<Mesh>(), object->getComponent<Material>(), data.getShader());
		
		return object;
	}
public:
	std::shared_ptr<QOpenGLFunctions> functions;

	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<LightSource>> lights;
	std::vector<std::shared_ptr<Object>> transparentObjects;
	std::shared_ptr<Object> lightSourceBlock;
	
	std::shared_ptr<Background> backround;

	GLCamera camera;
	
	Scene() = default;
	virtual void init() = 0;

	Scene(std::shared_ptr<QOpenGLFunctions> _functions): functions(std::move(_functions))
	{
		createLightSourceBlock();
	}
	void addModel(const std::vector<MeshLoader::LoadedModel>& models, const QVector3D& pos, ShaderData& data, const std::string& tag = "")
	{
		for (const auto& model : models)
		{
			objects.push_back(createObject(model, pos, data, tag));
		}
	}
	void addLight(std::shared_ptr<LightSource> light)
	{
		lights.push_back(std::move(light));
	}
	void addTransparent(const std::vector<MeshLoader::LoadedModel>& models, const QVector3D& pos, ShaderData& data, const std::string& tag = "")
	{
		for (const auto& model : models)
		{
			transparentObjects.push_back(createObject(model, pos, data, tag));
		}
	}
};
