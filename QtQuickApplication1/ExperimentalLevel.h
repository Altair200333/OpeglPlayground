#pragma once
#include "FPSCounter.h"
#include "Level.h"
#include "OnUpdateSubscriber.h"

class ExperimentalLevel: public Level
{
public:
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> wings;
	std::shared_ptr<Object> aill;
	std::shared_ptr<Object> ailr;

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
			ComponentManager::getComponent<Transform>(aill)->rotate(0.2, QVector3D(1, 0, 0));
			ComponentManager::getComponent<Transform>(ailr)->rotate(0.2, QVector3D(1, 0, 0));
		}
		if (Input::keyPressed(Qt::Key_G))
		{
			ComponentManager::getComponent<Transform>(aill)->rotate(-0.2, QVector3D(1, 0, 0));
			ComponentManager::getComponent<Transform>(ailr)->rotate(-0.2, QVector3D(1, 0, 0));
		}
		for (auto& light : lights)
		{
			light->position = QQuaternion::fromAxisAndAngle({ 0,1,0 }, 20.0f * FPSCounter::getFrameTime()) * light->position;
		}
	}

	void init() override
	{
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 6.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		//
		addTransparent(MeshLoader().loadModel("Assets/Models/earthAtmo.obj"), { 0, 3, 0 }, ShaderCollection::shaders["cubicCloud"]);
		
		addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		addLight(std::make_shared<DirectionalLight>());
		//
		backround = std::make_shared<Background>(functions, "Assets\\Models\\textures\\background.jpg");
		addModel(MeshLoader().loadModel("Assets/Models/fus.obj"), { 3.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		fuselage = objects.back();
		
		addModel(MeshLoader().loadModel("Assets/Models/wing.obj"), { 3.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		wings = objects.back();

		addModel(MeshLoader().loadModel("Assets/Models/ail.obj"), { 3.5f+1.08, 3+0.358, 0-1}, ShaderCollection::shaders["normals"]);
		aill = objects.back();
		addModel(MeshLoader().loadModel("Assets/Models/ail.obj"), { 3.5f- 1.08, 3 + 0.358, 0-1 }, ShaderCollection::shaders["normals"]);
		ailr = objects.back();
		
		fuselage->children.push_back(wings);
		fuselage->children.push_back(aill);
		fuselage->children.push_back(ailr);
	}
};
