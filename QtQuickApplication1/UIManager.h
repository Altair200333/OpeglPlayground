#pragma once
#include <QSlider>
#include <QLabel>
#include "GLWindow.h"
#include <QBoxLayout>
#include <QColorDialog>
#include <QPushButton>
#include <QFont>

class UIManager final: public QObject
{
public:
	QLabel* label;

	std::shared_ptr<QBoxLayout> verticalBox;
	std::shared_ptr<GLWindow> viewport;
	
	Window* window;
	void setWindow(Window* _window)
	{
		window = _window;
	}
	UIManager() = default;

	void init()
	{
		verticalBox = std::make_shared<QBoxLayout>(QBoxLayout::TopToBottom);
		viewport = std::make_shared<GLWindow>(window);
		viewport->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
		
		QSurfaceFormat format;
		format.setSamples(16);
		format.setVersion(3, 3);
		format.setProfile(QSurfaceFormat::CoreProfile);

		viewport->setFormat(format);

		//---
		label = new QLabel("framerate: xx");
		QFont f("Arial", 16, QFont::Bold);
		label->setFont(f);
		label->setStyleSheet("color: blue; background-color : white;");
		label->setMaximumHeight(20);
		
		//---
		verticalBox->setContentsMargins(0, 0, 0, 0);
		verticalBox->addWidget(viewport.get());
		verticalBox->addWidget(label);
		
		window->setLayout(verticalBox.get());
	}

};