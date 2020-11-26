#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H
#include "sShape.h"
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QObject>


class ScribbleArea : public QWidget, public QObject
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    ScribbleArea(QWidget* parent = 0) ;

    // Handles all events
    bool openImage(const QString& fileName);
    bool saveImage(const QString& fileName, const char* fileFormat);
    void setPenColor(const QColor& newColor) { myPenColor = newColor; }
    void setPenWidth(int newWidth) {myPenWidth = newWidth;}

    // Has the image been modified since last save
    bool isModified() const { return modified; }
    bool isScribbling() const { return scribbling; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }

    //bool lineMode;
    int nNodes;
    QPoint point1, point2;
    enum ScribbleModes { NONE, LINE, SINGLE, CYLINDER } scribblemodes = NONE;
    QList<sShape*> shapes;
    bool creatingShape;
    void updateShapes();

    void setLineMode() { updateShapes();  nNodes = 2; scribblemodes = LINE; creatingShape = true; }
    void setScribbleMode() { updateShapes(); scribblemodes = NONE; creatingShape = true;}
    void setSingleMode() { updateShapes(); scribblemodes = SINGLE; creatingShape = true;}
    void setCylinderMode() { updateShapes(); scribblemodes = CYLINDER; creatingShape = true;}

signals:
    void dataReady(const QList<sShape*>& shapes_);

public slots:

    // Events to handle
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // Updates the scribble area where we are painting
    void paintEvent(QPaintEvent* event) override;

    // Makes sure the area we are drawing on remains
    // as large as the widget
    void resizeEvent(QResizeEvent* event) override;

public: // for test
    void drawCylTo(QPoint &point, qreal WIDTH);
    void drawRectangle(QPoint& point);
    void drawLineTo(const QPoint& endPoint);
    void drawLineBetween(const QPoint& startPoint, const QPoint& endPoint);
private:
    void resizeImage(QImage* image, const QSize& newSize);

    // Will be marked true or false depending on if
    // we have saved after a change
    bool modified;

    // Marked true or false depending on if the user
    // is drawing
    bool scribbling;

    // Holds the current pen width & color
    int myPenWidth;
    QColor myPenColor;

    // Stores the image being drawn
    QImage image;

    // Stores the location at the current mouse event
    QPoint lastPoint;
};

#endif