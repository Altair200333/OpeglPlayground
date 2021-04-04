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
		
		if(Input::keyPressed(Qt::Key_R))
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
		if(selectedObject!=nullptr && Input::keyPressed(Qt::Key_G))
		{
			ComponentManager::getComponent<Transform>(selectedObject)->translate(camera->right* MouseInput::delta().x()*0.01f+
				camera->up * MouseInput::delta().y() * 0.01f);

		}
		//ComponentManager::getComponent<RigidBody>(cube)->body->applyForceToCenterOfMass(reactphysics3d::Vector3(1, 0, 0));

		btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);


		auto tr = ComponentManager::getComponent<Transform>(cube);
		trans.getOpenGLMatrix(tr->transform.data());
	}

	btCollisionShape* groundShape;
	btCollisionShape* fallShape;
	btRigidBody* groundRigidBody;
	btRigidBody* fallRigidBody;
	void initPhysics()
	{
		groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

		fallShape = new btSphereShape(1);

		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		btRigidBody::btRigidBodyConstructionInfo
			groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
		groundRigidBody = new btRigidBody(groundRigidBodyCI);
		PhysicsWorld::getWorld().addRigidBody(groundRigidBody);

		auto tr = ComponentManager::getComponent<Transform>(cube);
		btTransform transfrom = btTransform();
		transfrom.setFromOpenGLMatrix(tr->transform.data());
		
		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(transfrom);
		btScalar mass = 1;
		btVector3 fallInertia(0, 0, 0);
		fallShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(1, fallMotionState, fallShape, fallInertia);
		fallRigidBody = new btRigidBody(fallRigidBodyCI);
		PhysicsWorld::getWorld().addRigidBody(fallRigidBody);
		
	}
	void init() override
	{
		camera = std::make_shared<FreeCamera>();
		
		addModel(MeshLoader().loadModel("Assets/Models/ico1.obj"), { 0.5f, 5, 0 }, ShaderCollection::shaders["normals"]);
		
		//-----
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 10.5f, 20, 0 }, ShaderCollection::shaders["normals"]);
		cube = objects.back();
		
		addTransparent(MeshLoader().loadModel("Assets/Models/earthAtmo.obj"), { 0, 3, 0 }, ShaderCollection::shaders["cubicCloud"]);
		
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		dirLight = (std::make_shared<DirectionalLight>());
		addLight(dirLight);
		//

		backround = std::make_shared<SkyBackground>(functions);
		backround->light = dirLight;
		

		addModel(MeshLoader().loadModel("Assets/Models/plane.obj"), { 0, 0, 0 }, ShaderCollection::shaders["normals"]);
		plane = objects.back();

		loadPlane();
		//cube->addChild(fuselage);

		map = HeightMapGenerator().genHeightMap(200, 200, 0, 3);
		auto terr = HeightMapMeshGenerator().genMesh(map);
		addModel(MeshLoader::LoadedModel{ terr, std::make_shared<Material>() }, { 0, 0, 0 }, ShaderCollection::shaders["normals"]);

		auto surf = objects.back();
		
		initPhysics();

		//---
		sprites.push_back(std::make_shared<Sprite>("Assets\\Sprites\\UV_1k.jpg", 200, 200, 100, 100));

	}
	
	std::shared_ptr<HeightMap> map;
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
		ComponentManager::getComponent<Material>(transparentObjects.back())->alpha = 0.4f;
	}
};
