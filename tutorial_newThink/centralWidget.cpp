#include "centralWidget.h"

CentralWidget::CentralWidget(QWidget* parent)
    : QWidget(parent) {

    QGridLayout* controlsLayout = new QGridLayout(this);
    scribbleArea = new ScribbleArea(this);

    clearBut = new QPushButton("Clear", this);
    updateBut = new QPushButton("Update", this);
    crtLineBut = new QPushButton("Create line", this);
    crtScribbleBut = new QPushButton("Scribble", this);
    crtSingleBut = new QPushButton("Create 1 point", this);

    sListWidget = new QListWidget(this);

    controlsLayout->addWidget(scribbleArea, 0, 0);
    controlsLayout->addWidget(crtScribbleBut, 0,1);
    controlsLayout->addWidget(crtSingleBut, 1,1);
    controlsLayout->addWidget(crtLineBut, 2,1);
    controlsLayout->addWidget(updateBut, 3,1);
    controlsLayout->addWidget(clearBut, 4,1);
    controlsLayout->addWidget(sListWidget, 0,2);

    setLayout(controlsLayout);

}