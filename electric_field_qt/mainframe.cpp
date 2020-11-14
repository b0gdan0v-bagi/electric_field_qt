#include "mainframe.h"
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

MainFrame::MainFrame(QWidget* parent)
    : QMainWindow(parent) {

    QPixmap newpix("new.png");
    QPixmap openpix("open.png");
    QPixmap quitpix("quit.png");

    QAction* quit = new QAction("&Quit", this);

    QMenu* file;
    file = menuBar()->addMenu("&File");
    file->addAction(quit);

    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    QToolBar* toolbar = addToolBar("main toolbar");
    toolbar->addAction(QIcon(newpix), "New File");
    toolbar->addAction(QIcon(openpix), "Open File");
    toolbar->addSeparator();

    QAction* quit2 = toolbar->addAction(QIcon(quitpix), "Quit Application");
    connect(quit2, &QAction::triggered, qApp, &QApplication::quit);

    ListWidget* lw = new ListWidget(this);
    //ListWidget* lw2 = new ListWidget(this);
    setCentralWidget(lw);
    

    statusBar()->showMessage("Ready"); // показываем в нижней панели приложения сообщение "Ready"
}