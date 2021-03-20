#pragma once
#include "ComponentManager.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "SimpleMeshRenderer.h"
#include "Transform.h"

class Object
{
	ComponentManager componentManager;
public:
	template<typename T>
	void addComponent(std::shared_ptr<T> component)
	{
		componentManager.addComponent(component, this);
	}

	template<typename T>
	auto getComponent() const
	{
		return componentManager.getComponent<T>();
	}
	std::string tag;
	
	virtual ~Object() = default;
};
