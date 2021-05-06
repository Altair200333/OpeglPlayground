#pragma once
class Module
{
public:
	virtual ~Module() = default;
	bool enabled = true;
	virtual void update() = 0;
};