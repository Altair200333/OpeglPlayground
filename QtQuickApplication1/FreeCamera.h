#pragma once
#include "GLCamera.h"
#include "Input.h"
#include "MouseInput.h"
#include "OnUpdateSubscriber.h"
class FreeCamera: public GLCamera, public OnUpdateSubscriber
{
	std::map<int, QVector3D> controls = {
			   {Qt::Key_W, {0,0,1}},
			   {Qt::Key_S, {0,0,-1}},
			   {Qt::Key_A, {-1,0,0}},
			   {Qt::Key_D, {1,0,0}},
			   {Qt::Key_E, {0,1,0}},
			   {Qt::Key_Q, {0,-1,0}} };
public:
	FreeCamera()
	{}
	void onUpdate() override
	{
		for (auto& [key, dir] : controls)
		{
			if (Input::keyPressed(key))
			{
				translate(dir * 0.1f);
			}
		}

		if (MouseInput::keyPressed(Qt::LeftButton))
			look(MouseInput::delta().x() * 0.5f, MouseInput::delta().y() * 0.5f);
	}
};