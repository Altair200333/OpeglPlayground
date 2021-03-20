#pragma once

class EventSubscriber
{
public:
	EventSubscriber();
	virtual ~EventSubscriber();
};

#include "EventDispatcher.h"
inline EventSubscriber::EventSubscriber()
{
	EventDispatcher::subscribe(this);
}

inline EventSubscriber::~EventSubscriber()
{
	EventDispatcher::unsubscribe(this);
}
