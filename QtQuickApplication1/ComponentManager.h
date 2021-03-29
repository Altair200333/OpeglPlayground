#pragma once
#include <map>
#include "Object.h"
#include "ObjectComponentManager.h"

class ComponentManager final
{
	static ComponentManager& instance()
	{
		static ComponentManager manager;
		return manager;
	}
public:
	std::unordered_map<Object*, ObjectComponentManager> objectComponents;

	template<typename T>
	static std::shared_ptr<T> addComponent(const std::shared_ptr<Object>& object, std::shared_ptr<T> component)
	{
		auto& inst = instance();
		inst.objectComponents[object.get()].addComponent(component, object.get());
		return component;
	}

	template<typename T>
	static std::shared_ptr<T> getComponent(const std::shared_ptr<Object>& object)
	{
		auto& inst = instance();
		auto it = inst.objectComponents.find(object.get());
		if (it == inst.objectComponents.end())
			return {};
		return it->second.getComponent<T>();
	}
	template<typename T>
	static std::shared_ptr<T> getComponent(Object* object)
	{
		auto& inst = instance();
		auto it = inst.objectComponents.find(object);
		if (it == inst.objectComponents.end())
			return {};
		return it->second.getComponent<T>();
	}
};
