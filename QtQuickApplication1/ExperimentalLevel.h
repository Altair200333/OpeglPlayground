#pragma once
#include "FPSCounter.h"
#include "Level.h"
#include "OnUpdateSubscriber.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "SkyBackground.h"
#include "reactphysics3d/reactphysics3d.h"

class ExperimentalLevel: public Level
{
public:
	
	
	std::shared_ptr<Object> cube;
	std::shared_ptr<Object> plane;
	
	std::shared_ptr<DirectionalLight> dirLight;
	
	std::shared_ptr<Object> selectedObject = nullptr;

	ExperimentalLevel(QOpenGLFunctions* _functions) :Level(_functions)
	{

	}
	void onUpdate() override
	{
		
		for (auto& light : lights)
		{
			light->position = QQuaternion::fromAxisAndAngle({ 0,1,0 }, 20.0f * FPSCounter::getFrameTime()) * light->position;
		}
		
		if(MouseInput::keyPressed(Qt::RightButton))
		{
			//dirLight->direction = QQuaternion::fromAxisAndAngle(camera.right, MouseInput::delta().y()) * dirLight->direction;
			//dirLight->direction = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -MouseInput::delta().x()) * dirLight->direction;
			if(selectedObject)
			{
				auto transform = ComponentManager::getComponent<Transform>(selectedObject);
				transform->rotate(MouseInput::delta().y(), transform->right);
				transform->rotate(MouseInput::delta().x(), QVector3D(0,1,0));
			}
		}

		if(MouseInput::keyJustPressed(Qt::LeftButton) && pickedObjectId != -1 && pickedObjectId<objects.size())
		{
			selectedObject = objects[pickedObjectId];
		}
		if(selectedObject!=nullptr && MouseInput::keyPressed(Qt::MiddleButton))
		{
			//ComponentManager::getComponent<Transform>(selectedObject)->translate(camera.right* MouseInput::delta().x()*0.01f+
			//	camera.up * MouseInput::delta().y() * 0.01f);

			ComponentManager::getComponent<RigidBody>(cube)->body->applyForceToCenterOfMass(reactphysics3d::Vector3(MouseInput::delta().x(), MouseInput::delta().y(), 0)*4);
		}
		//ComponentManager::getComponent<RigidBody>(cube)->body->applyForceToCenterOfMass(reactphysics3d::Vector3(1, 0, 0));
	}

	
	void init() override
	{
		PhysicsWorld::init();
		
		//-----
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 2.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		cube = objects.back();
		auto rb1 = ComponentManager::addComponent(cube, std::make_shared<RigidBody>());
		rb1->init();
		const reactphysics3d::Vector3 halfExtents(1, 1, 1);
		// Create the box shape
		reactphysics3d::BoxShape* boxShape = PhysicsWorld::getCommon().createBoxShape(halfExtents);
		rb1->setCollider(boxShape, reactphysics3d::Transform::identity());
		
		//
		addTransparent(MeshLoader().loadModel("Assets/Models/earthAtmo.obj"), { 0, 3, 0 }, ShaderCollection::shaders["cubicCloud"]);
		
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//
		//backround = std::make_shared<ImageBackground>(functions, "Assets\\Models\\textures\\background.jpg");
		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;
		

		addModel(MeshLoader().loadModel("Assets/Models/plane.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		plane = objects.back();
		auto rb2 = ComponentManager::addComponent(plane, std::make_shared<RigidBody>());
		rb2->init(RigidBody::Type::KINEMATIC);
		const reactphysics3d::Vector3 halfExtents2(4, 0.1f, 4);
		// Create the box shape
		reactphysics3d::BoxShape* boxShape2 = PhysicsWorld::getCommon().createBoxShape(halfExtents2);
		rb2->setCollider(boxShape2, reactphysics3d::Transform::identity());

		loadPlane();
		//---
		sprites.push_back(std::make_shared<Sprite>("Assets\\Sprites\\UV_1k.jpg", 200, 200));
	}
	std::shared_ptr<Object> fuselage;
	
	void loadPlane()
	{
		addModel(MeshLoader().loadModel("Assets/Models/plane/body.obj"), { 3.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		fuselage = objects.back();

		addModel(MeshLoader().loadModel("Assets/Models/plane/body2.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());
		addModel(MeshLoader().loadModel("Assets/Models/plane/wings.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());
		addModel(MeshLoader().loadModel("Assets/Models/plane/engine.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());
		addModel(MeshLoader().loadModel("Assets/Models/plane/rud.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());
		addModel(MeshLoader().loadModel("Assets/Models/plane/eleron.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(objects.back());

		addTransparent(MeshLoader().loadModel("Assets/Models/plane/cockpit.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		fuselage->addChild(transparentObjects.back());
		ComponentManager::getComponent<Material>(transparentObjects.back())->alpha = 0.2f;
	}
};
