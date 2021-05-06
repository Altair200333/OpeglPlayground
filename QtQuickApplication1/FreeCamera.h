#pragma once
#include "GLCamera.h"
#include "Input.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
#include <cmath>

class FreeCamera: public GLCamera, public OnUpdateSubscriber
{
	std::map<int, QVector3D> controls = {
			   {Qt::Key_W, {0,0,1}},
			   {Qt::Key_S, {0,0,-1}},
			   {Qt::Key_A, {-1,0,0}},
			   {Qt::Key_D, {1,0,0}},
			   {Qt::Key_E, {0,1,0}},
			   {Qt::Key_Q, {0,-1,0}} };
	float minZoom;
	float maxZoom = 20;
	float acc = 0;
	float zoomTime = 0.25;
	static float lerp(float a, float b, float f)
	{
		f = std::clamp(f, 0.0f, 1.0f);
		return a + f * (b - a);
	}
public:
	FreeCamera()
	{
		minZoom = FOV;
	}
	
	void onUpdate() override
	{
		if (!enabled)
			return;
		if (MouseInput::keyPressed(Qt::RightButton))
		{
			acc += FPSCounter::getFrameTime();
			FOV = lerp(minZoom, maxZoom, acc/zoomTime);
		}
		else
		{
			acc -= FPSCounter::getFrameTime();
			FOV = lerp(minZoom, maxZoom, acc / zoomTime);
		}
		acc = std::clamp(acc, 0.0f, zoomTime);
		for (auto& [key, dir] : controls)
		{
			if (Input::keyPressed(key))
			{
				translate(dir * 0.1f);
			}
		}

		if (MouseInput::keyPressed(Qt::LeftButton))
			look(MouseInput::delta().x() * 0.13f, MouseInput::delta().y() * 0.13f);
	}
};