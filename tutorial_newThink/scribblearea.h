#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H
#include "sShape.h"
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QObject>
#include <vector>
#include "qmatrix.h"

#include <QMessageBox>
#include <QtWidgets>
#include "qcolor.h"
#include <algorithm>
#include "qdebug.h"



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
    QVector2D tempPoint; // for line
    enum ScribbleModes { NONE, LINE, SINGLE, CYLINDER, POTHEATMAP, EQPOTLINES, DIRECTIONS } scribblemodes = NONE;
    QList<sShape*> shapes;
    bool creatingShape;
    void updateShapes();

    void setLineMode() { updateShapes();  nNodes = 2; scribblemodes = LINE; creatingShape = true; }
    void setScribbleMode() { updateShapes(); scribblemodes = NONE; creatingShape = true;}
    void setSingleMode() { updateShapes(); scribblemodes = SINGLE; creatingShape = true;}
    void setCylinderMode() { updateShapes(); scribblemodes = CYLINDER; creatingShape = true;}
    void setEqPotLinesMode() { scribblemodes = EQPOTLINES; creatingShape = true;}
    void setDirectionsMode() { scribblemodes = DIRECTIONS; }


    std::vector<std::vector<float>> arrayOfPotencials;
    void calculatePotencial();
    QColor floatToRgb(float minValue, float maxValue, float value);

    bool findEqvivalent = { false };
    float minPot;
    float avgPot;
    float maxPot;
    void calcEqPot(QPoint& point);
    void drawPowerLine(QVector2D& startPos, const int maxPts = 10000, const QColor& lineColor = Qt::black, const bool reverse = false);

    void drawLines(QVector<QVector2D>& pointsToDraw, const QColor& lineColor = Qt::black);
    bool drawPowerLinesAroundCharge(const QVector2D &chargePoint);
    float chargeToAdd = { 1 };
    QVector2D summaryFieldInPoint(const QVector2D start, bool reverse = false);
    QVector2D plusFieldInPointByPoint(const QVector2D pos, const QVector2D chargePoint, const float charge);
    bool powerLineCrossChargeOrBorder(const QVector2D lastPoint);

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
    void drawText(const QPoint& point, const QString& s, const QColor c = Qt::black);
    void drawCylTo(QPoint &point, qreal WIDTH);
    void drawRectangle(QPoint& point, QColor pointColor = Qt::green, qreal pointWidth = 15);
    void drawLineTo(const QPoint& endPoint);
    void drawLineBetween(const QPoint& startPoint, const QPoint& endPoint, const QColor lineColor = Qt::black);
    void drawArrow(const QVector2D& fromPoint, const QVector2D& toPoint, const float head_length = 10.f, const float head_width = 5.f, const QColor lineColor = Qt::black, const bool drawBody = true);
public:
    void drawElFieldAllArea();
    bool drawElField = { false };
    int drawElFieldScale = { 50 };
public:
    void drawPotencialAllArea();
private:
    QVector2D findIntersectLineNormal(QVector2D line_p1, QVector2D line_p2, QVector2D outer_p);
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