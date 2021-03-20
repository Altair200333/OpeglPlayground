#pragma once
#include "LightSource.h"

class PointLight final: public LightSource
{
public:

	PointLight(QVector3D _position = { 0,0,0 }, QColor _color = { 250,250,250 }, float _intensity = 1): LightSource(_position, _color, _intensity)
	{	
	}

	void uploadToShader(std::shared_ptr<QOpenGLShaderProgram> shader, size_t index) override
	{
		shader->setUniformValue(("lights[" + std::to_string(index) + "].position").c_str(), position);
		shader->setUniformValue(("lights[" + std::to_string(index) + "].color").c_str(), color);
		shader->setUniformValue(("lights[" + std::to_string(index) + "].intensity").c_str(), intensity);
	}
};
