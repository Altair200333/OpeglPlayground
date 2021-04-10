#pragma once
#include "GLCamera.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
#include "Transform.h"

class FollowCamera final: public GLCamera, public OnUpdateSubscriber
{
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
	std::shared_ptr<Transform> target;
	FollowCamera()
	{
		FOV = 80;
		minZoom = FOV;

	}

	QVector3D getPosition() const override
	{
		return target->transform * position;
	}

	QVector3D getRight() const override
	{
		return target->getRotationTransform() * right;
	}
	QVector3D getForward() const override
	{
		return target->getRotationTransform() * front;
	}
	QVector3D getUp() const override
	{
		return target->getRotationTransform() * up;
	}
	void onUpdate() override
	{
		if (!enabled)
			return;
		if (MouseInput::keyPressed(Qt::RightButton))
		{
			acc += FPSCounter::getFrameTime();
			FOV = lerp(minZoom, maxZoom, acc / zoomTime);
		}
		else
		{
			acc -= FPSCounter::getFrameTime();
			FOV = lerp(minZoom, maxZoom, acc / zoomTime);
		}
		acc = std::clamp(acc, 0.0f, zoomTime);
		if (MouseInput::keyPressed(Qt::LeftButton))
			look(MouseInput::delta().x() * 0.13f, MouseInput::delta().y() * 0.13f);
	}
};
