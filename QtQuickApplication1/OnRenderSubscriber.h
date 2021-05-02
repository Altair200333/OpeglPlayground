#pragma once
#include "EventSubscriber.h"
#include "RenderContext.h"

class OnRenderSubscriber : virtual public EventSubscriber
{
public:
	virtual void onRender(const RenderContext& _context) = 0;
};
