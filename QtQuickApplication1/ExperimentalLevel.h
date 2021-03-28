#pragma once
#include "FPSCounter.h"
#include "Level.h"
#include "OnUpdateSubscriber.h"
#include "SkyBackground.h"

class ExperimentalLevel: public Level
{
public:
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> wings;
	std::shared_ptr<Object> aill;
	std::shared_ptr<Object> ailr;
	
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
		
	}

	void init() override
	{
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 6.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
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
