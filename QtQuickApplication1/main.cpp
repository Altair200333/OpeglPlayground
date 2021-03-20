#include <QGuiApplication>
#include <QApplication>
#include "GLWindow.h"
#include <QLabel>
#include <QStyleFactory>

#include "AloyApplication.h"
#include "QColorDialog"
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Window window;
    window.resize(1600, 800);
    AloyApplication aloyApp(&window);
    window.show();
  
    return app.exec();
}