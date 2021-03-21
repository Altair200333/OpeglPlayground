#pragma once
#include "LightSource.h"
#include <QColor>

class DirectionalLight: public LightSource
{
public:
	QVector3D direction;
	DirectionalLight(QColor _color = {250, 250, 250}, QVector3D _direction = { 0.2, -1, 0.2 }, float _intensity = 1) : LightSource(
		QVector3D(0,0,0), _color, _intensity)
	{
		direction = _direction;
		direction.normalize();
	}

	void uploadToShader(std::shared_ptr<QOpenGLShaderProgram> shader, size_t index) override
	{
		shader->setUniformValue(("dirLights[" + std::to_string(index) + "].color").c_str(), color);
		shader->setUniformValue(("dirLights[" + std::to_string(index) + "].direction").c_str(), direction);
		shader->setUniformValue(("dirLights[" + std::to_string(index) + "].intensity").c_str(), intensity);
	}
};
