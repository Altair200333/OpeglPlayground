#pragma once
#include <memory>
#include <QOpenGLShaderProgram>

class LightSource
{
public:
	QVector3D position = { 0,0,0 };
	QColor color = { 250,250,250 };
	float intensity = 1;

	LightSource(QVector3D _position = { 0,0,0 }, QColor _color = { 250,250,250 }, float _intensity = 1) :position(_position), color(_color),
		intensity(_intensity)
	{
	}
	virtual ~LightSource() = default;

	virtual void uploadToShader(std::shared_ptr<QOpenGLShaderProgram> shader, size_t index) = 0;
};
