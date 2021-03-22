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

#include "ExperimentalLevel.h"
#include "FPSCounter.h"
#include "Level.h"
#include "UIManager.h"

class AloyApplication final: public OnUpdateSubscriber
{
public:
	std::shared_ptr<Scene> activeLevel;
	UIManager manager;

	bool initialized = false;
	SceneRenderer renderer;
	
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
			renderer.nextDrawMode();
		
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
	}

	void render()
	{
		const auto retinaScale = manager.viewport->devicePixelRatio();
		manager.viewport->glViewport(0, 0, manager.viewport->width() * retinaScale, manager.viewport->height() * retinaScale);

		manager.viewport->glClearColor(1.0f, 1.0f, 1.0f, 1);
		manager.viewport->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(MouseInput::keyPressed(Qt::RightButton))
		{
			QSurfaceFormat format;
			
			manager.viewport->setFormat(format);
			renderer.renderPickShader(activeLevel);

			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			
			unsigned char data[4] = {1,1,1,0};
			manager.viewport->glReadPixels(MouseInput::getPosition().x(), MouseInput::getPosition().y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			
			const int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

			std::string col = std::to_string(data[0]) + " " + std::to_string(data[1]) + " "+std::to_string(data[2]) + " "
				+ std::to_string(data[3]) +"; " + std::to_string(pickedID);
			manager.label->setText(QString(col.c_str()));
		}
		else
		{
			renderer.render(activeLevel);
		}
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
