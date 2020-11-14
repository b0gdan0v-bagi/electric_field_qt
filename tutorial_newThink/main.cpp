#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    // The main application
    QApplication app(argc, argv);

    // Create and open the main window
    MainWindow window;
    window.show();
    window.resize(1280, 720);

    // Display the main window
    return app.exec();
}