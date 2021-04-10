#pragma once
#include "GLCamera.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
#include "Transform.h"

class FollowCamera final: public GLCamera, public OnUpdateSubscriber
{
public:
	std::shared_ptr<Transform> target;
	FollowCamera() = default;

	QVector3D getPosition() const override
	{
		return target->transform * position;
	}

	QQuaternion getDirMatrix() const
	{
		QQuaternion q = QQuaternion::fromRotationMatrix(target->transform.normalMatrix());
		return q;

	}
	
	QVector3D getRight() const override
	{
		return getDirMatrix() * right;
	}
	QVector3D getForward() const override
	{
		return getDirMatrix() * front;
	}
	QVector3D getUp() const override
	{
		return getDirMatrix() * up;
	}
	void onUpdate() override
	{
		if (!enabled)
			return;
		
		if (MouseInput::keyPressed(Qt::LeftButton))
			look(MouseInput::delta().x() * 0.13f, MouseInput::delta().y() * 0.13f);
	}
};
