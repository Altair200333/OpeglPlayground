#pragma once
#include "FPSCounter.h"
#include "HeightMap.h"
#include "HeightMapGenerator.h"
#include "HeightMapMeshGenerator.h"
#include "Level.h"
#include "OnUpdateSubscriber.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "SkyBackground.h"
#include "FreeCamera.h"
#include "b3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "CollisionShapeGenerator.h"
#include "FPSFollowCamera.h"
#include "Su33.h"

class ExperimentalLevel: public Level
{
public:
	
	
	std::shared_ptr<Object> cube;
	std::shared_ptr<Object> water;
	std::shared_ptr<Object> terrain;
	
	std::shared_ptr<DirectionalLight> dirLight;
	
	std::shared_ptr<Object> selectedObject = nullptr;

	std::shared_ptr<GLCamera> freeCamera;
	ExperimentalLevel(QOpenGLFunctions* _functions) :Level(_functions)
	{

	}

	void controlPlane()
	{
		//if(planeCamera->enabled)
		
	}
	int cameraState = 0;
	void onUpdate() override
	{
		
		for (auto& light : lights)
		{
			light->position = QQuaternion::fromAxisAndAngle({ 0,1,0 }, 20.0f * FPSCounter::getFrameTime()) * light->position;
		}
		
		if(Input::keyPressed(Qt::Key_R))
		{
			if(selectedObject)
			{
				auto transform = ComponentManager::getComponent<Transform>(selectedObject);
				transform->rotate(MouseInput::delta().y(), transform->right);
				transform->rotate(MouseInput::delta().x(), QVector3D(0,1,0));
			}
		}
		controlPlane();
		if(MouseInput::keyJustPressed(Qt::LeftButton) && pickedObjectId != -1 && pickedObjectId<objects.size())
		{
			selectedObject = objects[pickedObjectId];
		}
		if(selectedObject!=nullptr && Input::keyPressed(Qt::Key_G))
		{
			ComponentManager::getComponent<Transform>(selectedObject)->translate(camera->right* MouseInput::delta().x()*0.01f+
				camera->up * MouseInput::delta().y() * 0.01f);

		}
		if(MouseInput::keyPressed(Qt::MiddleButton) && selectedObject != nullptr)
		{
			auto selRb = ComponentManager::getComponent<RigidBody>(selectedObject);
			if(selRb)
				selRb->addForce(camera->right * MouseInput::delta().x()*5 +
					camera->up * MouseInput::delta().y()*5);
		}
		if(Input::keyJustPressed(Qt::Key_V))
		{
			cameraState = (++cameraState) % 3;
			plane->fpsCamera->enabled = false;
			plane->chaseCamera->enabled = false;
			freeCamera->enabled = false;
			
			if(cameraState == 0)
			{
				camera = freeCamera;
			}
			else if(cameraState == 1)
			{
				camera = plane->fpsCamera;
			}
			else if (cameraState == 2)
			{
				camera = plane->chaseCamera;
			}
			camera->enabled = true;
		}
	}


	void init() override
	{
		freeCamera = std::make_shared<FreeCamera>();
		
		camera = freeCamera;
		
		addModel(MeshLoader().loadModel("Assets/Models/ico1.obj"), { 0.5f, 5, 0 }, ShaderCollection::shaders["normals"]);
		ComponentManager::addComponent(objects.back(), 
			std::make_shared<RigidBody>())->init(
			CollisionShapeGenerator::getMeshCollider("Assets/Models/ico1.obj"), 1);
		//-----
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 10.5f, 20, 0 }, ShaderCollection::shaders["normals"]);
		cube = objects.back();
		ComponentManager::addComponent(cube, std::make_shared<RigidBody>())->init(ComponentManager::getComponent<Transform>(cube),
			CollisionShapeGenerator::getBox({ 1,1,1 }), 1);
		
		addModel(MeshLoader().loadModel("Assets/Models/flat.obj"), { 0, 0, 0 }, ShaderCollection::shaders["water"]);
		water = objects.back();
		ComponentManager::addComponent(water, std::make_shared<RigidBody>())->init(ComponentManager::getComponent<Transform>(water),
			CollisionShapeGenerator::getPlane({ 0,1,0 }), 0);
		
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//

		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;
		

		loadPlane();

		
		map = HeightMapGenerator().genHeightMap(200, 200, 0, 3);
		auto terr = HeightMapMeshGenerator().genMesh(map);
		addModel(MeshLoader::LoadedModel{ terr, std::make_shared<Material>() }, { 0, 2, 0 }, ShaderCollection::shaders["normals"]);
		terrain = objects.back();

		ComponentManager::addComponent(terrain, std::make_shared<RigidBody>())->init(CollisionShapeGenerator::getTerrain(map), 0);
		

		//---
		sprites.push_back(std::make_shared<Sprite>("Assets\\Sprites\\UV_1k.jpg", 200, 200, 100, 100));

	}
	
	std::shared_ptr<HeightMap> map;

	std::shared_ptr<Su33> plane;
	void loadPlane()
	{
		plane = std::make_shared<Su33>();
		plane->init(this);
		
	}
};
