#pragma once
#include <qmap.h>

class Input final
{
	QMap<int, bool> keys;
	QMap<int, bool> pressedKeys;
	static Input& instance()
	{
		static Input input;
		return input;
	}
public:
	static void pressKey(int keyCode)
	{
		instance().keys[keyCode] = true;
		instance().pressedKeys[keyCode] = true;
	}
	static void releaseKey(int keyCode)
	{
		instance().keys[keyCode] = false;
	}
	static bool keyPressed(int keyCode)
	{
		return instance().keys[keyCode];
	}
	static bool keyJustPressed(int keyCode)
	{
		return instance().pressedKeys[keyCode];
	}
	static void reset()
	{
		instance().pressedKeys.clear();
	}
};
