#pragma once
#include "Scene.h"
#include "OnUpdateSubscriber.h"

class Level: public Scene, public OnUpdateSubscriber
{
public:
	virtual ~Level() = default;
	Level(QOpenGLFunctions* _functions):Scene(_functions)
	{	
	}

};
