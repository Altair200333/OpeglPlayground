#pragma once
#include <qmatrix4x4.h>
#include "Component.h"


class Transform final: public Component
{
public:
	QMatrix4x4 transform;
	QVector3D position;
	QVector3D right = QVector3D(1,0,0);
	QVector3D up = QVector3D(0,1,0);
	QVector3D forward = QVector3D(0, 0, -1);

	void translate(const QVector3D& translation);

	void rotate(float angle, const QVector3D& axis);

	void rotate(const QQuaternion& quaternion);

	void rotateAround(float angle, const QVector3D& axis, const QVector3D& point);

	QQuaternion getRotationTransform() const
	{
		return QQuaternion::fromRotationMatrix(transform.normalMatrix());
	}
	
	QMatrix4x4 getGlobalTransform() const
	{
		Object* current = owner->parent;

		std::vector<QMatrix4x4> matrix;
		matrix.push_back(transform);

		while (current != nullptr)
		{
			matrix.push_back(ComponentManager::getComponent<Transform>(current)->transform);
			current = current->parent;
		}

		QMatrix4x4 model = QMatrix4x4();
		model.setToIdentity();

		for (int i = matrix.size() - 1; i >= 0; --i)
		{
			model *= matrix[i];
		}
		return model;
	}
	
	void reset()
	{
		position = QVector3D(0, 0, 0);
		transform.setToIdentity();
	}
	QVector3D getRight() const
	{
		return getRotationTransform() * right;
	}
	QVector3D getForward() const
	{
		return getRotationTransform() * forward;
	}
	QVector3D getUp() const
	{
		return getRotationTransform() * up;
	}
};

inline void Transform::translate(const QVector3D& translation)
{
	position += translation;
	QMatrix4x4 m;
	m.translate(translation);
	transform = m * transform;
}

inline void Transform::rotate(float angle, const QVector3D& axis)
{
	transform.rotate(QQuaternion::fromAxisAndAngle(axis, angle));
	
}

inline void Transform::rotate(const QQuaternion& quaternion)
{
	QMatrix4x4 m;
	m.translate(position);
	m.rotate(quaternion);
	m.translate(-position);

	transform = m * transform;
}

inline void Transform::rotateAround(float angle, const QVector3D& axis, const QVector3D& point)
{
	const QQuaternion rotation = QQuaternion::fromAxisAndAngle(axis, angle);

	QMatrix4x4 m;
	m.translate(point);
	m.rotate(rotation);
	m.translate(-point);

	position = m * position;
	
	transform = m * transform;
}
