#pragma once
#include <string>
#include <QOpenGLTexture>

struct Texture final
{
    std::string type;
    std::string path;
    QOpenGLTexture* texture;
	
	Texture() = default;
	Texture(const std::string& _path)
	{
		path = _path;
		texture = new QOpenGLTexture(QImage(QString(path.c_str())));
	}
	Texture(QOpenGLTexture* _texture):texture(_texture)
	{
	}
	
	enum FilterMode
	{
		Point,
		Bilinear,
		Trilinear,
		Anisotropic
	};
	
	enum WrapMode
	{
		Repeat,
		Clamp
	};
	
	void setFilterMode(int filterMode)
	{
		auto minFilter = GL_NEAREST;
		auto magFilter = GL_NEAREST;
		bool useAniso = false;
		if(filterMode== Point)
		{
			minFilter = GL_NEAREST;
			magFilter = GL_NEAREST;
		}
		else if (filterMode == Bilinear)
		{
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
			magFilter = GL_LINEAR_MIPMAP_NEAREST;
		
		}
		else if (filterMode == Trilinear)
		{
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_LINEAR;
		}
		else if (filterMode == Anisotropic)
		{
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_LINEAR;
			useAniso = true;
		}
		texture->bind();
		
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		
		if(useAniso)
		{
			float ans = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ans);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ans);
		}
		else
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0);
		}
	}
};
