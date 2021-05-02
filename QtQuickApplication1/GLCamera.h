#pragma once
#include <qmatrix4x4.h>
#include <qvector3d.h>
#include <QtMath>

class GLCamera
{
public:
	float FOV = 60;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float aspectRatio = 4.0f / 3.0f;
	
	QVector3D front = { 0, 0, 1 };
	QVector3D right = { 1, 0, 0 };
	QVector3D up =	  { 0, 1, 0 };
	QVector3D position = {0,0, 3};
	bool enabled = true;
	GLCamera()
	{
		updateCameraVectors();
	}
	QMatrix4x4 getProjectionMatrix()
	{
		QMatrix4x4 projection;
		projection.perspective(FOV, aspectRatio, 0.1f, 1000.0f);
		return projection;
	}
	QMatrix4x4 getViewMatrix() const
	{
		QMatrix4x4 view;
		view.lookAt(getPosition(), (getPosition() + getForward()), getUp());
		return view;
	}
	virtual QVector3D getPosition() const
	{
		return position;
	}
	virtual QVector3D getRight() const
	{
		return right;
	}
	virtual QVector3D getForward() const
	{
		return front;
	}
	virtual QVector3D getUp() const
	{
		return up;
	}
	void updateCameraVectors()
	{
		QVector3D forward;

		forward.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
		forward.setY(sin(qDegreesToRadians(pitch)));
		forward.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
		front = (forward).normalized();

		right = QVector3D::crossProduct(front, QVector3D(0,1,0)).normalized();
		up = QVector3D::crossProduct(right, front).normalized();
	}
	void translate(const QVector3D& mult)
	{
		position += front * mult.z();
		position += right * mult.x();
		position += up * mult.y();
		updateCameraVectors();
	}
	void look(float dx, float dy)
	{
		yaw += dx;
		pitch += dy;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		pitch = std::clamp(pitch, -89.0f, 89.0f);
		updateCameraVectors();
	}
};
