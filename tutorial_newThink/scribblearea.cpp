#include <QtWidgets>
#include "qcolor.h"
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif

#include "qdebug.h"

#include "scribblearea.h"
#include <algorithm>

ScribbleArea::ScribbleArea(QWidget* parent)
    : QWidget(parent), QObject(parent)
{
    // Roots the widget to the top left even if resized
    setAttribute(Qt::WA_StaticContents);

    // Set defaults for the monitored variables
    modified = false;
    scribbling = false;
    creatingShape = false;
    myPenWidth = 1;
    myPenColor = Qt::blue;
    scribblemodes = NONE; // test
    nNodes = 2;
    setMouseTracking(true);
    //drawRectangle(QPoint(this->size().width(), this->size().height()));
    //drawRectangle(QPoint(10, 10));
    //this->setBaseSize(500, 500);
    //shapes.push_back(new sPoint(QPoint(this->size().width() / 2, this->size().height() / 2)));
    //for (int i=0;i<width();i++)
    //    for (int j=0;j<height();j++)
            //shapes.push_back(new sPoint(QPoint(i*50 , j*50 )));
    
    //QColor(Qrbg 2);
    /*int testx_1 = 100 + width() / 4;
    int testx_2 = 200 + 3*width() / 4;
    int testy_1 = 100+10*height() / 2;
    int testy_2 = 100 + 10*height() / 2;
    shapes.push_back(new sPoint(QPoint(testx_1, testy_1)));
    shapes.push_back(new sPoint(QPoint(testx_2, testy_2), -1.f));
    */

    updateShapes();
}



void ScribbleArea::updateShapes()
{
    clearImage();
    for (auto const& sh : shapes)
    {
        switch (sh->shapeType)
        {
        case 0: {drawRectangle(sh->vecNodes[0].pos.toPoint()); break; }
        case 1: {
            for (int i = 0; i < sh->vecNodes.size(); i++) drawRectangle(sh->vecNodes.at(i).pos.toPoint());
            // "if" for checking only
            if (sh->vecNodes.size() > 1) drawLineBetween(sh->vecNodes.at(0).pos.toPoint(), sh->vecNodes.at(1).pos.toPoint());
            break; }
        }
    }
}

void ScribbleArea::calculatePotencial()
{
    arrayOfPotencials.clear();
    float radius;
    float minPot = 0;
    float avgPot = 0;
    float maxPot = minPot;
    QMessageBox akaDebug;
    for (int y = 0; y < height(); y++)
    {
        std::vector<float> tempPot(width());
        for (int x = 0; x < width(); x++)
        {
            tempPot[x] = 0;
            for (auto const& sh : shapes) {
                switch (sh->shapeType)
                {
                case sShape::ShapeType::POINT: {
                     radius = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
                     if (radius > 3) tempPot[x] += (sh->charge / radius); break; 
                }
                case sShape::ShapeType::LINE: {
                    QVector2D intersectPoint;
                    float A1 = (sh->vecNodes[0].pos.y() - sh->vecNodes[1].pos.y());
                    float B1 = (sh->vecNodes[1].pos.x() - sh->vecNodes[0].pos.x());
                    float C1 = (sh->vecNodes[0].pos.x() * sh->vecNodes[1].pos.y() - sh->vecNodes[1].pos.x() * sh->vecNodes[0].pos.y());
                    float A2 = -B1;
                    float B2 = A1;
                    float C2 = B1 * x - A1 * y;
                    float det = A1 * B2 - B1 * A2;
                    if (det == 0) break;
                    else {
                        intersectPoint.setX((-C1 * B2 + B1 * C2) / det);
                        intersectPoint.setY((A2 * C1 - A1 * C2) / det);
                    }
                    
                    //drawRectangle(intersectPoint.toPoint());
                    //drawLineBetween(QPoint(x,y),intersectPoint.toPoint());
                    
                    //
                    radius = QVector2D(x, y).distanceToLine(sh->vecNodes[0].pos, sh->vecNodes[1].pos);
                    if (radius > 3)
                    {
                        float l1, l2;
                        float d1 = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
                        float d2 = QVector2D(x, y).distanceToPoint(sh->vecNodes[1].pos);
                        float l = (sh->vecNodes[1].pos - sh->vecNodes[0].pos).length();
                    
                        if (radius <= d1 && radius <= d2)
                        {
                            if (d1 < d2) {
                                //l1 = 0; l2 = l;
                            }
                            else {
                                //l2 = 0; l1 = l;
                            }
                        }
                        else
                        {
                            //l1 = intersectPoint.distanceToPoint(sh->vecNodes[0].pos);
                            //l2 = intersectPoint.distanceToPoint(sh->vecNodes[1].pos);
                        }
                        l1 = intersectPoint.distanceToPoint(sh->vecNodes[0].pos);
                        l2 = intersectPoint.distanceToPoint(sh->vecNodes[1].pos);
                        if (l1+l2>l)
                            if (d1 < d2)
                            {
                                l1 = 0; l2 = l;
                            }
                            else
                            {
                                l1 = l; l2 = 0;
                            }
                        
                        tempPot[x] += sh->charge * 1 * log((l1 + sqrt(radius * radius + l1 * l1)) / (-l2 + sqrt(radius * radius + l2 * l2))) /*/ l*/;
                        akaDebug.setText("l1 " + QString::number(l1)+ " l2 " + QString::number(l2) + " d1 " + QString::number(d1) + " d2 " + QString::number(d2));
                        //akaDebug.exec();
                    }
                    
                    break;
                }
                    
                }
               
            }
            if (tempPot[x] > maxPot) maxPot = tempPot[x];
            if (tempPot[x] < minPot) minPot = tempPot[x];
            avgPot += tempPot[x];
        }
        arrayOfPotencials.push_back(tempPot);
    }
    avgPot /= ( height() * width());
    QPainter painter(&image);
    
    akaDebug.setText("min = " + QString::number(minPot) + " max = " + QString::number(maxPot) + " avg " + QString::number(avgPot));
    akaDebug.exec();
    

    //float const maxV = (float)pow(256, 3);
    float const maxV = 255;
    for (int y = 0; y < height(); y++)
    {
        for (int x = 0; x < width(); x++)
        {
            
            //painter.setPen(QPen(QColor::fromRgb(maxV,
            //    maxV - maxV* (   (arrayOfPotencials[y][x] - minPot) / (maxPot - minPot)),
            float normalizedPot = (arrayOfPotencials[y][x] - minPot) / (maxPot - minPot);
            //QColor colorHeatMap = floatToRgb(0, 1, normalizedPot);
            QColor colorHeatMap = floatToRgb(minPot, maxPot/2, arrayOfPotencials[y][x]);

            painter.setPen(QPen(
                colorHeatMap,
                1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPoint(x, y);
        }
    }
    update();   
}

QColor ScribbleArea::floatToRgb(float minValue, float maxValue, float value) {
     float ratio = ratio = 2 * (value - minValue) / (maxValue - minValue);
     int red = (int)(std::max(0.f, 255 * (ratio - 1)));
     int blue = (int)(std::max(0.f, 255 * (1 - ratio)));
     int green = 255 - blue - red;
     return QColor( red,green,blue, 255);
    /*float r = calc;
    float g = (int)(calc * scale) % 1;
    r -= g / scale;
    float b = (int)(calc * scale * scale) % 1;
    g -= b / scale;
    return  QVector3D(r, g, b);*/

}

void ScribbleArea::calcEqPot(QPoint& point)
{
    //findEqvivalent = false;
    //QMessageBox akaDebug;
    //akaDebug.setText("x = " + QString::number(point.x()) + " y = " + QString::number(point.y()));
    //akaDebug.exec();
    QPainter painter(&image);
    painter.setPen(QPen(Qt::red,1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    float potToFind = arrayOfPotencials[point.y()][point.x()];
    float precision = 1; // %
    for (int y = 0; y < height(); y++)
        for (int x = 0; x < width(); x++)
        {
            if (potToFind > 0) {
                if (arrayOfPotencials[y][x] <= potToFind * (1 + precision / 100)
                    && arrayOfPotencials[y][x] >= potToFind * (1 - precision / 100))
                {
                    painter.drawPoint(x, y);
                }
            }
            else {
                if (arrayOfPotencials[y][x] >= potToFind * (1 + precision / 100)
                    && arrayOfPotencials[y][x] <= potToFind * (1 - precision / 100))
                {
                    painter.drawPoint(x, y);
                }
            }
        }
    update();
}

// If a mouse button is pressed check if it was the
// left button and if so store the current position
// Set that we are currently drawing
void ScribbleArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Make sure user we destroy line if user want to switch another thing while LINE havent finished
        if (scribblemodes != LINE) nNodes = 2; 

        switch (scribblemodes)
        {
        case ScribbleArea::NONE: {
            lastPoint = event->pos();
            scribbling = true;
            break; }
        case ScribbleArea::LINE:
        {
            if (nNodes == 2)
            {
                tempPoint = QVector2D(event->pos());// 
                drawRectangle(tempPoint.toPoint());
                nNodes--;
                scribbling = true;
                setMouseTracking(true);
            }
            else if (nNodes == 1)
            {
                nNodes++;
                shapes.push_back(new sLine(tempPoint, QVector2D(event->pos())));
                scribbling = false;
                //setMouseTracking(false);
                updateShapes();
            }
            break; 
        }
        case ScribbleArea::SINGLE: {
            shapes.push_back(new sPoint(QVector2D(event->pos())));
            updateShapes();
            break; }
        case ScribbleArea::CYLINDER:
            break;
        case ScribbleArea::POTHEATMAP:  break;
        case ScribbleArea::EQPOTLINES: {calcEqPot(event->pos()); break; }
           
        case ScribbleArea::DIRECTIONS:
            break;
        }
        emit dataReady(shapes);
    }
}

// When the mouse moves if the left button is clicked
// we call the drawline function which draws a line
// from the last position to the current
void ScribbleArea::mouseMoveEvent(QMouseEvent* event)
{
    switch (scribblemodes)
    {
    case ScribbleArea::NONE: {
        if ((event->buttons() & Qt::LeftButton) && scribbling) drawLineTo(event->pos());
        break; }
    case ScribbleArea::LINE: {
        if (nNodes == 1)
        {
            updateShapes();
            drawRectangle(tempPoint.toPoint());
            drawLineBetween(tempPoint.toPoint(), event->pos());
            drawRectangle(event->pos());
        }
        else {
            updateShapes(); 
            drawRectangle(event->pos());
        }
        break; }
    case ScribbleArea::SINGLE: {updateShapes(); drawRectangle(event->pos()); break; }
    case ScribbleArea::CYLINDER:
        break;
    case ScribbleArea::POTHEATMAP:
        break;
    case ScribbleArea::EQPOTLINES:
        break;
    case ScribbleArea::DIRECTIONS:
        break;
    }
}

// If the button is released we set variables to stop drawing
void ScribbleArea::mouseReleaseEvent(QMouseEvent* event)
{
    if (scribblemodes == NONE)
    {
        if (event->button() == Qt::LeftButton && scribbling) {
            drawLineTo(event->pos());
            scribbling = false;
        }
    }
}

// QPainter provides functions to draw on the widget
// The QPaintEvent is sent to widgets that need to
// update themselves
void ScribbleArea::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Returns the rectangle that needs to be updated
    QRect dirtyRect = event->rect();

    // Draws the rectangle where the image needs to
    // be updated
    painter.drawImage(dirtyRect, image, dirtyRect);
}

// Resize the image to slightly larger then the main window
// to cut down on the need to resize the image
void ScribbleArea::resizeEvent(QResizeEvent* event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void ScribbleArea::drawCylTo(QPoint &point, qreal WIDTH = 10)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, WIDTH, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    QPoint m_point = point;

    painter.drawRoundRect(QRect(point,point+point),50,50);
    //painter.drawRoundRect()
    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update();
}

void ScribbleArea::drawRectangle(QPoint& point)
{
    QPainter painter(&image);
    painter.setPen(QPen(Qt::green, 15, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    //QPoint m_point = point;

    painter.drawPoint(point);
    //painter.drawRoundRect(QRect(point, point + point), 50, 50);
    //painter.drawRoundRect()
    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update();
}

void ScribbleArea::drawLineTo(const QPoint& endPoint)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));

    // Draw a line from the last registered point to the current
    painter.drawLine(lastPoint, endPoint);

    // Set that the image hasn't been saved
    modified = true;

    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update(QRect(lastPoint, endPoint).normalized()
        .adjusted(-rad, -rad, +rad, +rad));

    // Update the last position where we left off drawing
    lastPoint = endPoint;
}

void ScribbleArea::drawLineBetween(const QPoint& startPoint, const QPoint& endPoint)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));

    // Draw a line from the last registered point to the current
    painter.drawLine(startPoint, endPoint);

    // Set that the image hasn't been saved
    modified = true;

    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update(QRect(startPoint, endPoint).normalized()
        .adjusted(-rad, -rad, +rad, +rad));

    // Update the last position where we left off drawing
    lastPoint = endPoint;
}

// When the app is resized create a new image using
// the changes made to the image
void ScribbleArea::resizeImage(QImage* image, const QSize& newSize)
{
    // Check if we need to redraw the image
    if (image->size() == newSize)
        return;

    // Create a new image to display and fill it with white
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));

    // Draw the image
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

