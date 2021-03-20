#pragma once
#include <type_traits>

class EventSubscriber;

template<typename T, std::enable_if_t<std::is_base_of<EventSubscriber, T>::value, bool> = true>
class Event
{
public:
	virtual void handle(T* var) const = 0;
	virtual ~Event() = default;
};