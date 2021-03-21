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

class GLWindow final : public QOpenGLWidget, public QOpenGLFunctions
{
public:
	QTimer* timer = nullptr;
	GLWindow(QWidget* parent = nullptr): QOpenGLWidget(parent)
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->start(0);
		
		setMouseTracking(true);   //without this command input goes insane if mouse pointer is outside the window
		setFocusPolicy(Qt::ClickFocus);//allow widget to be focused
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
		QOpenGLWidget::keyPressEvent(e);
	}

	void keyReleaseEvent(QKeyEvent* e) override
	{
		Input::releaseKey(e->nativeVirtualKey());
		QOpenGLWidget::keyReleaseEvent(e);
	}

	void mouseMoveEvent(QMouseEvent* e) override
	{
		MouseInput::mouseCallback(e->pos());
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
};
