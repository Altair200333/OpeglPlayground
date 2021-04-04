#pragma once
#include "Level.h"
#include "FPSCounter.h"
#include "FreeCamera.h"
#include "HeightMap.h"
#include "HeightMapGenerator.h"
#include "HeightMapMeshGenerator.h"
class TexturesTaskLevel : public Level
{
public:
	std::shared_ptr<DirectionalLight> dirLight;

	TexturesTaskLevel(QOpenGLFunctions* _functions): Level(_functions)
	{

	}
	void onUpdate() override
	{
		if (MouseInput::keyPressed(Qt::MiddleButton))
		{
			dirLight->direction = QQuaternion::fromAxisAndAngle(camera->right, MouseInput::delta().y()*0.5f) * dirLight->direction;
			dirLight->direction = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -MouseInput::delta().x() * 0.5f) * dirLight->direction;
		}
	}
	std::shared_ptr<Object> road1;
	std::shared_ptr<Object> road2;
	std::shared_ptr<Object> road3;
	std::shared_ptr<Object> road4;
	void init() override
	{
		camera = std::make_shared<FreeCamera>();

		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//

		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;

		addModel(MeshLoader().loadModel("Assets/Models/task/road1.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		road1 = objects.back();
		ComponentManager::getComponent<Material>(road1)->textures[0].setFilterMode(Texture::FilterMode::Point);
		//----
		addModel(MeshLoader().loadModel("Assets/Models/task/road1.obj"), { 0, 0, 2 }, ShaderCollection::shaders["normals"]);
		road2 = objects.back();
		ComponentManager::getComponent<Material>(road2)->textures[0].setFilterMode(Texture::FilterMode::Bilinear);

		addModel(MeshLoader().loadModel("Assets/Models/task/road1.obj"), { 0, 0, 4 }, ShaderCollection::shaders["normals"]);
		road3 = objects.back();
		ComponentManager::getComponent<Material>(road3)->textures[0].setFilterMode(Texture::FilterMode::Trilinear);

		addModel(MeshLoader().loadModel("Assets/Models/task/road1.obj"), { 0, 0, 6 }, ShaderCollection::shaders["normals"]);
		road4 = objects.back();
		ComponentManager::getComponent<Material>(road4)->textures[0].setFilterMode(Texture::FilterMode::Anisotropic);
	}
	
};