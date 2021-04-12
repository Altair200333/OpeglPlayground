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
#include "FollowCamera.h"

class ExperimentalLevel: public Level
{
public:
	
	
	std::shared_ptr<Object> cube;
	std::shared_ptr<Object> plane;
	std::shared_ptr<Object> terrain;
	
	std::shared_ptr<DirectionalLight> dirLight;
	
	std::shared_ptr<Object> selectedObject = nullptr;

	std::shared_ptr<GLCamera> freeCamera;
	std::shared_ptr<GLCamera> planeCamera;
	ExperimentalLevel(QOpenGLFunctions* _functions) :Level(_functions)
	{

	}

	void controlPlane()
	{
		//if(planeCamera->enabled)
		{
			auto rb = ComponentManager::getComponent<RigidBody>(fuselage);
			auto transform = ComponentManager::getComponent<Transform>(fuselage);
			if (Input::keyPressed(Qt::Key_Shift))
			{
				rb->addForce(transform->getForward() * 20);
			}
			if (Input::keyPressed(Qt::Key_Control))
			{
				rb->addForce(-transform->getForward() * 20);
			}

			if(MouseInput::keyPressed(Qt::MiddleButton))
			{
				rb->addTorgue(transform->getRight() * MouseInput::delta().y()*10);
				rb->addTorgue(transform->getForward() * MouseInput::delta().x()*5);
			}
		}
	}

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
			if(freeCamera->enabled)
			{
				camera = planeCamera;
			}
			else
			{
				camera = freeCamera;
			}
			freeCamera->enabled = !freeCamera->enabled;
			planeCamera->enabled = !planeCamera->enabled;
		}
		auto fuselageQuat = ComponentManager::getComponent<Transform>(fuselage)->getRotationTransform();
		auto navBallTransform = ComponentManager::getComponent<Transform>(navball);
		QMatrix4x4 navTransform;
		navTransform.translate(-0.074943, -0.024163, -2.41394);
		navBallTransform->transform = navTransform;
		navBallTransform->rotate(fuselageQuat.inverted());
	
	}


	void init() override
	{
		freeCamera = std::make_shared<FreeCamera>();
		planeCamera = std::make_shared<FollowCamera>();
		planeCamera->enabled = false;
		planeCamera->position = QVector3D(0, 0.29, -1.9);
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
		
		
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//

		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;
		

		loadPlane();

		std::dynamic_pointer_cast<FollowCamera>(planeCamera)->target = ComponentManager::getComponent<Transform>(fuselage);
		
		map = HeightMapGenerator().genHeightMap(200, 200, 0, 3);
		auto terr = HeightMapMeshGenerator().genMesh(map);
		addModel(MeshLoader::LoadedModel{ terr, std::make_shared<Material>() }, { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		terrain = objects.back();

		ComponentManager::addComponent(terrain, std::make_shared<RigidBody>())->init(CollisionShapeGenerator::getTerrain(map), 0);
		

		//---
		sprites.push_back(std::make_shared<Sprite>("Assets\\Sprites\\UV_1k.jpg", 200, 200, 100, 100));

	}
	
	std::shared_ptr<HeightMap> map;
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> navball;
	
	void loadPlane()
	{
		addModel(MeshLoader().loadModel("Assets/Models/plane/body.obj"), { 1.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		fuselage = objects.back();
		ComponentManager::addComponent(fuselage, std::make_shared<RigidBody>())->init(
			CollisionShapeGenerator::getCompoundShape(
		{
				CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/body_c.obj"),
				CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/wings_c.obj")
			}), 1);

		ComponentManager::getComponent<RigidBody>(fuselage)->setGravity(QVector3D(0, -1.0f, 0));
		//--
		
		addModel(MeshLoader().loadModel("Assets/Models/plane/wings.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());
		addModel(MeshLoader().loadModel("Assets/Models/plane/engine.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());

		addModel(MeshLoader().loadModel("Assets/Models/plane/panel.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());

		addModel(MeshLoader().loadModel("Assets/Models/plane/seat.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());

		addModel(MeshLoader().loadModel("Assets/Models/plane/navball.obj"), { -0.074943, -0.024163, -2.41394 }, ShaderCollection::shaders["plain"]);
		navball = objects.back();
		fuselage->addChild(navball);
		
		addTransparent(MeshLoader().loadModel("Assets/Models/plane/cockpit.obj"), { 0, 0, 0 }, ShaderCollection::shaders["plain"]);
		fuselage->addChild(transparentObjects.back());
		ComponentManager::getComponent<Material>(transparentObjects.back())->alpha = 0.23f;
	}
};
