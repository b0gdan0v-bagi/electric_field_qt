#include "mousecoordlabel.h"

void MouseCoordLabel::mousePressEvent(QMouseEvent* ev)
{
    const QPoint p = ev->pos();
    emit mousePressed(p);
}

MouseCoordLabel::MouseCoordLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f) {}

MouseCoordLabel::MouseCoordLabel(const QString& text, QWidget* parent, Qt::WindowFlags f)
    : QLabel(text, parent, f) {}