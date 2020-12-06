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
        case sShape::ShapeType::POINT: {
            QColor chargeColor;
            chargeColor = (sh->charge > 0) ? Qt::red : Qt::blue;
            drawRectangle(sh->vecNodes[0].pos.toPoint(), chargeColor); break; }
        case sShape::ShapeType::LINE: {
            QColor chargeColor;
            chargeColor = (sh->charge > 0) ? Qt::red : Qt::blue;
            for (int i = 0; i < sh->vecNodes.size(); i++) drawRectangle(sh->vecNodes.at(i).pos.toPoint(), chargeColor);
            // "if" for checking only
            if (sh->vecNodes.size() > 1) drawLineBetween(sh->vecNodes.at(0).pos.toPoint(), sh->vecNodes.at(1).pos.toPoint(), Qt::black);
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
                    for (auto const& pts : sh->allPoints)
                    {
                        radius = QVector2D(x, y).distanceToPoint(*pts);
                        if (radius > 3) tempPot[x] += sh->charge / (radius * sh->allPoints.size());
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
            QColor colorHeatMap = floatToRgb(minPot, maxPot, arrayOfPotencials[y][x]);

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
                shapes.push_back(new sLine(tempPoint, QVector2D(event->pos()), chargeToAdd));
               // for (auto const& pts : shapes.back()->allPoints)
               //     drawRectangle(pts->toPoint());
                scribbling = false;
                //setMouseTracking(false);
                updateShapes();
            }
            break; 
        }
        case ScribbleArea::SINGLE: {
            shapes.push_back(new sPoint(QVector2D(event->pos()),chargeToAdd));
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
            drawLineBetween(tempPoint.toPoint(), event->pos(), Qt::blue);
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
    {
        QVector2D trueField(event->pos());
        float trueFieldValue = 0;
        updateShapes();
        float const koef = 100.f;
        for (auto const& sh : shapes)
            switch (sh->shapeType)
            {
            case sShape::ShapeType::POINT: {
                if ((QVector2D(event->pos()) - sh->vecNodes[0].pos).length() < 10)
                    drawRectangle(sh->vecNodes[0].pos.toPoint(), sh->getColor(), 25);

                QVector2D chargePoint = sh->vecNodes.back().pos;
                drawLineBetween(chargePoint.toPoint(), event->pos());
                float radius = (chargePoint - QVector2D(event->pos())).length();
                if (radius > 3) {
                    
                    float field = sh->charge / pow(radius / koef, 2);
                    float lengthOfFieldVector = (sh->charge > 0) ? 50 : -50;
                    float lambda = (radius+ lengthOfFieldVector)/radius;
                    QVector2D fieldPoint = (QVector2D(event->pos())-chargePoint)*lambda+chargePoint;
                    float minField = sh->charge / pow(qMax(this->width(), this->height()),2);
                    float maxField = sh->charge; // /radius = 1
                    QColor lineColor = floatToRgb(minField, maxField, field);
                    drawLineBetween(event->pos(), fieldPoint.toPoint(), lineColor);
                    lambda = (radius + lengthOfFieldVector) / radius;
                    trueField+= fieldPoint - QVector2D(event->pos());
                    trueFieldValue += field;
                }
                break; }
            case sShape::ShapeType::LINE:
            {
                for (auto const pts : sh->allPoints)
                {
                    float radius = (*pts - QVector2D(event->pos())).length();
                    if (radius > 3) {
                        trueFieldValue += sh->chargePerPoint / pow(radius / koef, 2);
                        float lengthOfFieldVector = (sh->charge > 0) ? 50 : -50;
                        float lambda = (radius + lengthOfFieldVector) / radius;
                        QVector2D fieldPoint = (QVector2D(event->pos()) - *pts) * lambda + *pts;
                        drawLineBetween(event->pos(), fieldPoint.toPoint());
                        trueField += fieldPoint - QVector2D(event->pos());

                        //res += plusFieldInPointByPoint(start, *pts, sh->chargePerPoint);
                    }
                }
                /*QPoint intersectPoint = findIntersectLineNormal(sh->vecNodes[0].pos, sh->vecNodes[1].pos, QVector2D(event->pos())).toPoint();
                float lengthToP1 = (sh->vecNodes[0].pos - QVector2D(intersectPoint)).length();
                float lengthToP2 = (sh->vecNodes[1].pos - QVector2D(intersectPoint)).length();
                float lengthLine = (sh->vecNodes[0].pos - sh->vecNodes[1].pos).length();
                if (lengthToP1 > lengthLine) drawLineBetween(intersectPoint, sh->vecNodes[1].pos.toPoint());
                if (lengthToP2 > lengthLine) drawLineBetween(intersectPoint, sh->vecNodes[0].pos.toPoint());
                drawLineBetween(intersectPoint, event->pos());
                drawRectangle(intersectPoint, Qt::black);*/
                
                break;
            }
            }
        //drawRectangle(event->pos(), Qt::black);
        float lengthOfFieldVector = (trueFieldValue > 0) ? 50 : -50;
        float radius = (trueField - QVector2D(event->pos())).length();
        float lambda = (radius + lengthOfFieldVector) / radius;
        QVector2D fieldPoint = ( trueField - QVector2D(event->pos())) * lambda + trueField;
        drawLineBetween(event->pos(), fieldPoint.toPoint(), Qt::red);
        drawText(fieldPoint.toPoint(), QString::number(trueFieldValue));
        drawText(QPoint(50,50), " x " + QString::number(event->pos().x()) + " y " + QString::number(event->pos().y()));
        QList<QVector2D> pointsOfPowerLine;
        
        pointsOfPowerLine.push_back(summaryFieldInPoint(QVector2D(event->pos())));
        int debugCount = 0;
        while (true) // a lot of breaks, all situations
        {
            QVector2D tempPoint = summaryFieldInPoint(pointsOfPowerLine.back());
            drawText(tempPoint.toPoint(), QString::number(debugCount));
            if (!powerLineCrossChargeOrBorder(tempPoint)) {
                drawRectangle(tempPoint.toPoint()); break;
            }

            pointsOfPowerLine.push_back(tempPoint);
            debugCount++;
            drawRectangle(tempPoint.toPoint(), Qt::darkMagenta, 5);
            if (debugCount > 100) {
                drawRectangle(tempPoint.toPoint(), Qt::darkRed); 
                drawText(tempPoint.toPoint(), QString::number(debugCount));
                break;
            }
        } 
        for (int i = 0; i < pointsOfPowerLine.size() - 1; i++) drawLineBetween(pointsOfPowerLine[i].toPoint(), pointsOfPowerLine[i + 1].toPoint(), Qt::darkGreen);
        /*pointsOfPowerLine.clear();
        pointsOfPowerLine.push_back(summaryFieldInPoint(QVector2D(event->pos())));
        while (true) // a lot of breaks, all situations
        {
            QVector2D tempPoint = summaryFieldInPoint(pointsOfPowerLine.back());

            if (!powerLineCrossChargeOrBorder(tempPoint)) break;

            pointsOfPowerLine.push_back(tempPoint);
            debugCount++;
            if (debugCount > 10000) break;
        }*/
        //for (int i = 0; i < pointsOfPowerLine.size() - 1; i++) drawLineBetween(pointsOfPowerLine[i].toPoint(), pointsOfPowerLine[i + 1].toPoint(), Qt::darkGreen);
        break;
    }
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

void ScribbleArea::drawText(const QPoint& point, const QString& s, QColor c)
{
    QPainter painter(&image);
    painter.setPen(QPen(c, myPenWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    painter.drawText(point, s);
    update();
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

void ScribbleArea::drawRectangle(QPoint& point, QColor pointColor, qreal pointWidth)
{
    QPainter painter(&image);
    painter.setPen(QPen(pointColor, pointWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    painter.drawPoint(point);
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

void ScribbleArea::drawLineBetween(const QPoint& startPoint, const QPoint& endPoint, const QColor lineColor)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(lineColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
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

