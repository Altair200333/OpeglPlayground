#pragma once
#include "GLCamera.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
#include "Transform.h"

class FollowCamera final: public GLCamera, public OnUpdateSubscriber
{
public:
	std::shared_ptr<Transform> target;
	FollowCamera()
	{
		FOV = 80;
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
		
		if (MouseInput::keyPressed(Qt::LeftButton))
			look(MouseInput::delta().x() * 0.13f, MouseInput::delta().y() * 0.13f);
	}
};
