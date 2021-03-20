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
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<MeshRenderer> renderer;
	std::shared_ptr<Material> material;
	
	void initRenderer(std::shared_ptr<QOpenGLFunctions> functions, const std::string& fragment = "Shaders/triangle.fs", const std::string& vertex = "Shaders/triangle.vs",
		const std::string& geometry = "")
	{
		renderer->initMeshRenderer(functions, transform, mesh, material, fragment, vertex, geometry);
	}
	void initRenderer(std::shared_ptr<QOpenGLFunctions> functions, std::shared_ptr<QOpenGLShaderProgram> shader)
	{
		renderer->initMeshRenderer(functions, transform, mesh, material, shader);
	}
	virtual ~Object() = default;
};
