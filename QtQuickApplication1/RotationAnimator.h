#pragma once
#include "FPSCounter.h"
#include "Transform.h"
#include "OnUpdateSubscriber.h"

class RotationAnimator : public OnUpdateSubscriber
{
public:
	std::shared_ptr<Transform> transform;
	QVector3D axis;

	float current = 0;
	float target = 0;

	float speed = 10.0f;

	void init(std::shared_ptr<Transform> _transform, QVector3D _axis, float _speed = 10)
	{
		axis = _axis;
		transform = _transform;
		speed = _speed;
	}
	void onUpdate() override
	{
		if (fabs(current - target) >= 0.1f)
		{
			const float delta = (target > current ? 1 : -1) * speed * FPSCounter::getFrameTime();
			transform->rotate(delta, axis);

			current += delta;
		}
	}
};
