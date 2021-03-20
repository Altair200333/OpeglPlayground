#pragma once
#include <memory>

#include <QWindow>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QPainter>

#include "Input.h"
#include "MouseInput.h"
#include <QKeyEvent>
#include <QOpenGLWidget>
#include <QBasicTimer>
#include <QMainWindow>
#include <QColorDialog>
#include <QTimer>

class Window : public QWidget
{
Q_OBJECT
public:
	Window()
	{
	}
};
