#pragma once
#include <string>
#include <QOpenGLTexture>

struct Texture final
{
    std::string type;
    std::string path;
    QOpenGLTexture* texture;
};
