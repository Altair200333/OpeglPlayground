#pragma once
#include "FPSCounter.h"
#include "Level.h"
#include "OnUpdateSubscriber.h"

class ExperimentalLevel: public Level
{
public:
	//std::shared_ptr<Object> atmosphere;

	ExperimentalLevel(QOpenGLFunctions* _functions) :Level(_functions)
	{

	}
	void onUpdate() override
	{
		//if (Input::keyJustPressed(Qt::Key_R))
		//	ComponentManager::getComponent<MeshRenderer>(atmosphere)->enabled = !ComponentManager::getComponent<MeshRenderer>(atmosphere)->enabled;

		for (auto& light : lights)
		{
			light->position = QQuaternion::fromAxisAndAngle({ 0,1,0 }, 20.0f * FPSCounter::getFrameTime()) * light->position;
		}
	}

	void init() override
	{
		addModel(MeshLoader().loadModel("Assets/Models/sam2.obj"), { 6.5f, 3, 0 }, ShaderCollection::shaders["normals"]);
		//
		//addTransparent(MeshLoader().loadModel("Assets/Models/earthAtmo.obj"), { 0, 3, 0 }, ShaderCollection::shaders["cubicCloud"]);
		//atmosphere = transparentObjects.back();
		//
		//addLight(std::make_shared<PointLight>(QVector3D{ -8, 4, 7 }, QColor{ 255, 255, 255 }, 2.5));
		//addLight(std::make_shared<PointLight>(QVector3D{ 30, 3, -7 }, QColor{ 255, 23, 12 }, 3));
		//
		backround = std::make_shared<Background>(functions, "Assets\\Models\\textures\\background.jpg");

	}
};
