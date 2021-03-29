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
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> wings;
	std::shared_ptr<Object> aill;
	std::shared_ptr<Object> ailr;
	
	std::shared_ptr<Object> cube;
	
	std::shared_ptr<DirectionalLight> dirLight;
	
	std::shared_ptr<Object> selectedObject = nullptr;

	ExperimentalLevel(QOpenGLFunctions* _functions) :Level(_functions)
	{

	}
	void onUpdate() override
	{
		if (Input::keyPressed(Qt::Key_R))
			ComponentManager::getComponent<Transform>(fuselage)->translate(QVector3D(0.1, 0, 0));
		if (Input::keyPressed(Qt::Key_T))
			ComponentManager::getComponent<Transform>(fuselage)->translate(QVector3D(-0.1, 0, 0));
		if(Input::keyPressed(Qt::Key_F))
		{
			ComponentManager::getComponent<Transform>(aill)->rotate(0.35, QVector3D(1, 0, 0));
			ComponentManager::getComponent<Transform>(ailr)->rotate(0.35, QVector3D(1, 0, 0));
		}
		if (Input::keyPressed(Qt::Key_G))
		{
			ComponentManager::getComponent<Transform>(aill)->rotate(-0.35, QVector3D(1, 0, 0));
			ComponentManager::getComponent<Transform>(ailr)->rotate(-0.35, QVector3D(1, 0, 0));
		}
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
			ComponentManager::getComponent<Transform>(selectedObject)->translate(camera.right* MouseInput::delta().x()*0.01f+
				camera.up * MouseInput::delta().y() * 0.01f);
		}
		//phys();
	}

	reactphysics3d::PhysicsWorld* world;
	reactphysics3d::PhysicsCommon* physicsCommon;
	
	reactphysics3d::Vector3 position;
	reactphysics3d::Quaternion orientation;
	reactphysics3d::Transform transform;
	
	reactphysics3d::RigidBody* body;
	const reactphysics3d::decimal timeStep = 1.0f / 120.0f;
	
	void phys()
	{
		body->applyForceToCenterOfMass(reactphysics3d::Vector3(0, 5, 0));

		world->update(timeStep);
		
		const reactphysics3d::Transform& transform = body->getTransform();
		const reactphysics3d::Vector3& position = transform.getPosition();
		auto cubeTransform = ComponentManager::getComponent<Transform>(cube);
		QMatrix4x4 matrix;
		matrix.translate(position.x, position.y, position.z);
		cubeTransform->transform = matrix;
	}
	void init() override
	{
		//physicsCommon = new reactphysics3d::PhysicsCommon();
		//world = physicsCommon->createPhysicsWorld();
		//orientation = reactphysics3d::Quaternion::identity();
		//position = reactphysics3d::Vector3(0, 20, 0);
		//body = world->createRigidBody(transform);
		//transform = reactphysics3d::Transform(position, orientation);

		PhysicsWorld::init();
		
		//-----
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 6.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		cube = objects.back();
		
		ComponentManager::addComponent(cube, std::make_shared<RigidBody>())->init();
		//
		addTransparent(MeshLoader().loadModel("Assets/Models/earthAtmo.obj"), { 0, 3, 0 }, ShaderCollection::shaders["cubicCloud"]);
		
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//
		//backround = std::make_shared<ImageBackground>(functions, "Assets\\Models\\textures\\background.jpg");
		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;
		
		addModel(MeshLoader().loadModel("Assets/Models/fus.obj"), { 3.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		fuselage = objects.back();
		
		addModel(MeshLoader().loadModel("Assets/Models/wing.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		wings = objects.back();

		addModel(MeshLoader().loadModel("Assets/Models/ail.obj"), { 1.08, 0.358, -1}, ShaderCollection::shaders["normals"]);
		aill = objects.back();
		addModel(MeshLoader().loadModel("Assets/Models/ail.obj"), { -1.08, 0.358, -1 }, ShaderCollection::shaders["normals"]);
		ailr = objects.back();

		fuselage->addChild(wings);
		fuselage->addChild(aill);
		fuselage->addChild(ailr);

		addModel(MeshLoader().loadModel("Assets/Models/plane.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);

		//---
		sprites.push_back(std::make_shared<Sprite>("Assets\\Sprites\\UV_1k.jpg", 200, 200));
	}
};
