#pragma once
#include "Event.h"
#include "OnUpdateSubscriber.h"

class OnUpdateEvent final : public Event<OnUpdateSubscriber>
{
public:
	void handle(OnUpdateSubscriber* var) const override
	{
		var->onUpdate();
	}
};
