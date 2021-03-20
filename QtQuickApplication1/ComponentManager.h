#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include "TypeInfo.h"
#include "Component.h"

class ComponentManager final
{
	template<typename T>
	using isComponent = std::enable_if_t<std::is_base_of<Component, T>::value, int>;
	
	std::unordered_map<TypeInfo::type, std::shared_ptr<Component>> components{};
public:
	
	template<typename T, isComponent<T>* = nullptr>
	void addComponent(std::shared_ptr<T> component, Object* owner)
	{
		std::dynamic_pointer_cast<Component>(component)->setOwner(owner);
		components.insert(std::make_pair(TypeInfo::type_name<T>(), std::move(component)));
	}
	
	template<typename T, isComponent<T>* = nullptr>
	[[nodiscard]] std::shared_ptr<T> getComponent() const
	{
		auto it = components.find(TypeInfo::type_name<T>());
		if(it == components.end())
			return {};
		return std::dynamic_pointer_cast<T>(it->second);
	}
};
