#pragma once
#include <qwidget.h>
#include <qlabel.h>

class MouseCoordLabel : public QLabel
{
    Q_OBJECT
signals:
    void mousePressed(const QPoint&);

public:
    MouseCoordLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
    MouseCoordLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);

    void mousePressEvent(QMouseEvent* ev);
};


