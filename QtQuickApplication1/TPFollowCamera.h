#pragma once
#include "GLCamera.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
#include "Transform.h"

class TPFollowCamera final : public GLCamera, public OnUpdateSubscriber
{
public:
	std::shared_ptr<Transform> target;
	QVector3D targetPos;
	QVector3D offset{ 0,1.5f,8 };
	QVector3D getPosition() const override
	{
		return position;
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
		targetPos = target->position - target->getForward()*offset.z()+target->getUp()*offset.y();
		position = position + (targetPos - position) * FPSCounter::getFrameTime()*10;
	}
};
