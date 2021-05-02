#pragma once
#pragma once
#include "Event.h"
#include "OnRenderSubscriber.h"
#include "RenderContext.h"

class OnRenderEvent final : public Event<OnRenderSubscriber>
{
public:
	RenderContext context;
	OnRenderEvent(const RenderContext& _context):context(_context)
	{
		
	}
	void handle(OnRenderSubscriber* var) const override
	{
		var->onRender(context);
	}
};
