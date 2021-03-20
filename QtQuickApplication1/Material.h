#pragma once
#include <QColor>
#include <QOpenGLShaderProgram>
#include "Component.h"
#include "Texture.h"

class Material final: public Component
{
public:
	std::vector<Texture> textures;
	std::vector<Texture> normal;
	std::vector<Texture> specular;
	
	QColor diffuse;
	QColor ambient = QColor(20, 200, 100);

	float roughness = 0.02f;
	bool isLightSource = false;
	

	Material(QColor _diffuse = QColor(255, 149, 100)):diffuse(_diffuse)
	{}
	void uploadToShader(std::shared_ptr<QOpenGLShaderProgram>& shader)
	{
		shader->setUniformValue("roughness", roughness);
		shader->setUniformValue("color", diffuse);
		shader->setUniformValue("ambient", ambient);
	}
};
