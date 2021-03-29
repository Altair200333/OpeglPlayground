#pragma once
#include "OnUpdateSubscriber.h"
#include "Scene.h"
#include <QColorDialog>
#include <QBoxLayout>
#include "GLWindow.h"
#include "Window.h"
#include <QSlider>
#include <QLabel>
#include "SceneRenderer.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <QOpenGLFramebufferObjectFormat>

#include "ExperimentalLevel.h"
#include "FPSCounter.h"
#include "GUIRender.h"
#include "Level.h"
#include "RenderLayerStack.h"
#include "UIManager.h"
#include "PickRenderer.h"
#include "PhysicsWorld.h"

class AloyApplication final: public OnUpdateSubscriber
{
public:
	std::shared_ptr<Scene> activeLevel;
	UIManager manager;

	bool initialized = false;
	std::shared_ptr<SceneRenderer> renderer;
	std::shared_ptr<PickRenderer> pickRenderer;
	std::shared_ptr<GUIRenderer> guiRenderer;
	RenderLayerStack renderStack;
	
	AloyApplication(Window* window)
	{
		manager.setWindow(window);
		manager.init();
	}

	void updateScene()
	{
		//stuff like this should be overwatched from application - the apex class
		activeLevel->camera.aspectRatio = static_cast<float>(manager.viewport->width()) / manager.viewport->height();
		if (Input::keyJustPressed(Qt::Key_Z))
			renderer->nextDrawMode();
		
		moveCamera();
	}

	void updateFrameRate()
	{
		FPSCounter::updateFps();
		
		std::stringstream ss;
		ss << "framerate: " << std::fixed << std::setprecision(2) << 1.0f / FPSCounter::getFPS();
		manager.label->setText(QString(ss.str().c_str()));
	}

	void onUpdate() override
	{
		if (!initialized)
		{
			init();
			initialized = true;
		}

		updateFrameRate();
		
		updateScene();
		
		render();
	}
	
	void init()
	{
		activeLevel = std::make_shared<ExperimentalLevel>(manager.viewport.get());
		manager.viewport->glEnable(GL_DEPTH_TEST);
		activeLevel->init();

		pickRenderer = std::make_shared<PickRenderer>();
		pickRenderer->scene = activeLevel;
		pickRenderer->viewport = manager.viewport;
		renderStack.layers.push_back(pickRenderer);
		
		renderer = std::make_shared<SceneRenderer>();
		renderer->scene = activeLevel;
		renderer->viewport = manager.viewport;
		renderStack.layers.push_back(renderer);

		guiRenderer = std::make_shared<GUIRenderer>();
		guiRenderer->scene = activeLevel;
		guiRenderer->viewport = manager.viewport;
		renderStack.layers.push_back(guiRenderer);

		PhysicsWorld::getWorld().setIsDebugRenderingEnabled(true);
	}
	

	void render()
	{
		const auto retinaScale = manager.viewport->devicePixelRatio();
		manager.viewport->glViewport(0, 0, manager.viewport->width() * retinaScale, manager.viewport->height() * retinaScale);

		manager.viewport->glClearColor(1.0f, 1.0f, 1.0f, 1);
		manager.viewport->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(MouseInput::keyPressed(Qt::LeftButton))
		{
			activeLevel->pickedObjectId = pickRenderer->pickedObjectId;
		}
		manager.viewport->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderStack.render();
		// Get a reference to the debug renderer
		reactphysics3d::DebugRenderer& debugRenderer = PhysicsWorld::getWorld().getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::
			DebugItem::CONTACT_POINT, true);
		debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer ::
			DebugItem::CONTACT_NORMAL, true);
		debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer ::
			DebugItem::COLLIDER_AABB, true);

	}

	void moveCamera()
	{
		std::map<int, QVector3D> controls = {
			{Qt::Key_W, {0,0,1}},
			{Qt::Key_S, {0,0,-1}},
			{Qt::Key_A, {-1,0,0}},
			{Qt::Key_D, {1,0,0}},
			{Qt::Key_E, {0,1,0}},
			{Qt::Key_Q, {0,-1,0}} };

		for (auto& [key, dir] : controls)
		{
			if (Input::keyPressed(key))
			{
				activeLevel->camera.translate(dir * 0.1f);
			}
		}
		
		if (MouseInput::keyPressed(Qt::LeftButton))
			activeLevel->camera.look(MouseInput::delta().x() * 0.5f, MouseInput::delta().y() * 0.5f);
	}
};
