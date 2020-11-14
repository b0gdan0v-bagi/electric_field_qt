#include "mainframe.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    MainFrame window;

    window.resize(1280, 720);
    window.setWindowTitle("Application skeleton");
    window.show();

    return app.exec();
}