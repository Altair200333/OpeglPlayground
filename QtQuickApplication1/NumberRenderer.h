#pragma once
#include "ComponentManager.h"
#include "MeshLoader.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "OnRenderSubscriber.h"
#include "ShaderCollection.h"
#include "ShaderData.h"

class NumberRenderer: public OnRenderSubscriber
{
public:
	QOpenGLFunctions* functions;
	std::vector<std::shared_ptr<Object>> digits;
	int number = 0;
	float spacing = 0.0044f;
	QVector3D position = QVector3D(0,0,0);
	NumberRenderer(QOpenGLFunctions* _functions):functions(_functions)
	{
		for(int i=0;i<10;++i)
		{
			digits.push_back(createObject(MeshLoader().loadModel("Assets/Models/plane/text/"+std::to_string(i)+".obj")[0], 
				{ 0, 0, 0 }, ShaderCollection::shaders["plain"]));
		}
	}
	void setParent(std::shared_ptr<Object> parent)
	{
		for (int i = 0; i < 10; ++i)
		{
			parent->addChild(digits[i]);
		}
	}
	void onRender(const RenderContext& _context) override
	{
		unsigned int digit = 0;
		int counter = 0;
		if (0 == number) {
			auto tr = ComponentManager::getComponent<Transform>(digits[0]);
			tr->translate(-tr->position);
			tr->translate(position);
			ComponentManager::getComponent<MeshRenderer>(digits[0])->render(_context);
		}
		else {
			digit = pow(10, 1.0 + floor(log10(number)));
			while (digit /= 10) {
				//doSomething((number / digit) % 10);
				auto tr = ComponentManager::getComponent<Transform>(digits[(number / digit) % 10]);
				tr->translate(-tr->position);
				tr->translate(position);
				tr->translate(QVector3D(spacing * counter, 0, 0));
				ComponentManager::getComponent<MeshRenderer>(digits[(number / digit) % 10])->render(_context);
				counter++;
			}
		}
	}
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
};
