#pragma once
#include "Window.h"

#include <memory>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QScreen>
#include <QOpenGLExtraFunctions>
#include <array>
#include <QColorDialog>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "GLCamera.h"
#include <QWindow>
#include "Input.h"
#include "MouseInput.h"
#include "Vertex.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "Scene.h"
#include <QMenuBar>
#include <QPushButton>
#include <QHBoxLayout>
#include "EventDispatcher.h"
#include "OnUpdateEvent.h"
#include "OnUpdateSubscriber.h"

class GLWindow final : public QOpenGLWidget, public QOpenGLFunctions, public OnUpdateSubscriber
{
public:
	QTimer* timer = nullptr;
	GLWindow(QWidget* parent = nullptr): QOpenGLWidget(parent)
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->start(0);
		
		setMouseTracking(true);   //without this command input goes insane if mouse pointer is outside the window
		setFocusPolicy(Qt::StrongFocus);//allow widget to be focused
	}
	
	void initializeGL() override
	{
		initializeOpenGLFunctions();
	}

	void paintGL() override
	{
		EventDispatcher::dispatch(OnUpdateEvent());
		
		Input::reset();
		MouseInput::reset();
	}
	virtual void onClose() {}

protected:
	bool event(QEvent* event) override
	{
		assert(event);
		switch (event->type())
		{
		case QEvent::Close:
			onClose();
			return QOpenGLWidget::event(event);
		default:
			return QOpenGLWidget::event(event);
		}
	}
	//bunch of callbacks for input
	void keyPressEvent(QKeyEvent* e) override
	{
		Input::pressKey(e->nativeVirtualKey());
		Input::pressKey(e->key());
		QOpenGLWidget::keyPressEvent(e);
	}

	void keyReleaseEvent(QKeyEvent* e) override
	{
		Input::releaseKey(e->nativeVirtualKey());
		Input::releaseKey(e->key());
		QOpenGLWidget::keyReleaseEvent(e);
	}
	bool lockMouse = true;
	void onUpdate() override
	{
		if(Input::keyJustPressed(Qt::Key_F1))
		{
			lockMouse = !lockMouse;
		}
	}
	void mouseMoveEvent(QMouseEvent* e) override
	{
		MouseInput::mouseCallback(e->pos());
		if(lockMouse)
		{
			lockPointer();
		}
		QOpenGLWidget::mouseMoveEvent(e);
	}

	void mousePressEvent(QMouseEvent* e) override
	{
		MouseInput::pressMouseKey(e->button());
		QOpenGLWidget::mousePressEvent(e);
	}

	void mouseReleaseEvent(QMouseEvent* e) override
	{
		MouseInput::releaseMouseKey(e->button());
		QOpenGLWidget::mouseReleaseEvent(e);
	}


	void lockPointer()
	{
		int w = width();
		int h = height();
		auto globalWP = mapToGlobal(pos());
		int x = globalWP.x();
		int y = globalWP.y();
		QCursor::setPos(x+w/2, y + h / 2);
		MouseInput::setPos(mapFromGlobal(QCursor::pos()));
	}

	void leaveEvent(QEvent* event) override
	{
		if(lockMouse)
		{
			lockPointer();
		}
	}
};
