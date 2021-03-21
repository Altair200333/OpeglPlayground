#pragma once
#include <qpoint.h>

class MouseInput final
{
	bool firstMouse = true;
	
	QPoint mousePos;
	QPoint deltaMouse;

	QMap<int, bool> mouseKeys;
	QMap<int, bool> mousePressedKeys;
	
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
		instance().mousePressedKeys.clear();
	}
	static QPoint delta()
	{
		return instance().deltaMouse;
	}
	//----
	static void pressMouseKey(int keyCode)
	{
		instance().mouseKeys[keyCode] = true;
		instance().mousePressedKeys[keyCode] = true;
	}
	static void releaseMouseKey(int keyCode)
	{
		instance().mouseKeys[keyCode] = false;
	}
	static bool keyPressed(int keyCode)
	{
		return instance().mouseKeys[keyCode];
	}
	static bool keyJustPressed(int keyCode)
	{
		return instance().mousePressedKeys[keyCode];
	}
	//----
};
