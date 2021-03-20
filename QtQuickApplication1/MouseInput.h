#pragma once
#include <qpoint.h>

class MouseInput final
{
	bool firstMouse = true;
	
	QPoint mousePos;
	QPoint deltaMouse;
	
	static MouseInput& instance()
	{
		static MouseInput input;
		return input;
	}
public:
	static void mouseCallback(const QPoint& _mousePos)
	{
		auto& inst = instance();
		if (inst.firstMouse)
		{
			inst.mousePos = _mousePos;
			inst.firstMouse = false;
		}
		inst.deltaMouse = _mousePos - inst.mousePos;
		inst.deltaMouse.setY(-inst.deltaMouse.y());
		inst.mousePos = _mousePos;
	}
	
	static void reset()
	{
		instance().deltaMouse = { 0,0 };
	}
	static QPoint delta()
	{
		return instance().deltaMouse;
	}
};
