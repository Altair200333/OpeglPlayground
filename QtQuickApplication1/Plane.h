#pragma once
#include "OnUpdateSubscriber.h"
#include "Scene.h"
class Plane: public OnUpdateSubscriber
{
public:
	virtual void init(Scene* scene) = 0;
	
};