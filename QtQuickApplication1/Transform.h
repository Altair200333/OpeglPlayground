#pragma once
#include <qmatrix4x4.h>
#include "Component.h"


class Transform final: public Component
{
public:
	QMatrix4x4 transform;
	QVector3D position;

	void translate(const QVector3D& translation);

	void rotate(float angle, const QVector3D& axis);

	void rotate(const QQuaternion& quaternion);

	void rotateAround(float angle, const QVector3D& axis, const QVector3D& point);

	void reset()
	{
		position = QVector3D(0, 0, 0);
		transform.setToIdentity();
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

	position -= point;
	position = rotation * position;
	position += point;

	transform = m * transform;
}
