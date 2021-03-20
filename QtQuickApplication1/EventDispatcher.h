#pragma once
#include <list>
#include "Event.h"

class EventSubscriber;
class EventDispatcher final
{
	std::list<EventSubscriber*> subscribers;
	static EventDispatcher& instance()
	{
		static EventDispatcher dispatcher;
		return dispatcher;
	}
public:
	static void subscribe(EventSubscriber* subscriber)
	{
		instance().subscribers.push_back(subscriber);
	}
	static void unsubscribe(EventSubscriber* subscriber)
	{
		instance().subscribers.remove(subscriber);
	}
	template<typename T>
	static void dispatch(const Event<T>& e);
};

#include "EventSubscriber.h"
template <typename T>
void EventDispatcher::dispatch(const Event<T>& e)
{
	for (auto& sub : instance().subscribers)
	{
		T* subscriberT = dynamic_cast<T*>(sub);
		if (subscriberT != nullptr)
		{
			e.handle(subscriberT);
		}
	}
}