#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <QObject>

#include "Input.h"
#include "MouseInput.h"
#include "Object.h"
#include <QTransform>


#include "ImageBackground.h"
#include "ComponentManager.h"
#include "MeshLoader.h"
#include "ShaderCollection.h"
#include "ShaderData.h"
#include "SkyBackground.h"
#include "Sprite.h"

class Scene
{
protected:
	void createLightSourceBlock()
	{
		lightSourceBlock = createObject(MeshLoader().loadModel("Assets/Models/ico.obj")[0], QVector3D(0, 0, 0),
			ShaderCollection::shaders["lightSource"]);
	}
public:

	std::shared_ptr<Object> createObject(const MeshLoader::LoadedModel& model, const QVector3D& pos, ShaderData& data, const std::string& tag = "") const
	{
		auto object = std::make_shared<Object>();
		object->tag = tag;
		
		ComponentManager::addComponent(object, std::make_shared<Transform>());
		ComponentManager::addComponent(object, model.mesh);
		ComponentManager::addComponent(object, model.material);
		ComponentManager::addComponent(object, data.renderer->getRenderer());

		ComponentManager::getComponent<Transform>(object)->translate(pos);
		
		ComponentManager::getComponent<MeshRenderer>(object)->initMeshRenderer(functions, data.getShader());
		
		return object;
	}
	QOpenGLFunctions* functions;

	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<LightSource>> lights;
	std::vector<std::shared_ptr<Object>> transparentObjects;
	std::shared_ptr<Object> lightSourceBlock;
	
	std::shared_ptr<SkyBackground> backround;
	std::vector<std::shared_ptr<Sprite>> sprites;

	int pickedObjectId = -1;

	
	std::shared_ptr<GLCamera> camera;
	
	Scene() = default;
	virtual void init() = 0;

	Scene(QOpenGLFunctions* _functions): functions(_functions)
	{
		camera = std::make_shared<GLCamera>();
		createLightSourceBlock();
	}
	void addModel(const std::vector<MeshLoader::LoadedModel>& models, const QVector3D& pos, ShaderData& data, const std::string& tag = "")
	{
		for (const auto& model : models)
		{
			objects.push_back(createObject(model, pos, data, tag));
		}
	}
	void addModel(const MeshLoader::LoadedModel& model, const QVector3D& pos, ShaderData& data, const std::string& tag = "")
	{
		objects.push_back(createObject(model, pos, data, tag));
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
