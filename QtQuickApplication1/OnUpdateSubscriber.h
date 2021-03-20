#pragma once
#include "EventSubscriber.h"

class OnUpdateSubscriber : virtual public EventSubscriber
{
public:
	virtual ~OnUpdateSubscriber() = default;
	virtual void onUpdate() = 0;
};
