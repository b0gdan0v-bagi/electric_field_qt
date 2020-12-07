#include "scribblearea.h"


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
        QVector2D mousePosVector(event->pos());
        float trueFieldValue = 0;
        updateShapes();
        QVector2D fieldPoint = summaryFieldInPoint(mousePosVector);
        fieldPoint -= mousePosVector;
        fieldPoint *= 50 / fieldPoint.length();
        fieldPoint += mousePosVector;
        drawLineBetween(mousePosVector.toPoint(), fieldPoint.toPoint(), Qt::red);
        drawArrow(mousePosVector, fieldPoint);

        QVector2D fieldPointReverse = summaryFieldInPoint(mousePosVector, true);
        fieldPointReverse -= mousePosVector;
        fieldPointReverse *= 50 / fieldPointReverse.length();
        fieldPointReverse += mousePosVector;
        //drawLineBetween(summaryFieldInPoint(QVector2D(event->pos())).toPoint(), event->pos());
        drawLineBetween(mousePosVector.toPoint(), fieldPointReverse.toPoint(), Qt::black);

        QVector<QVector2D> pointsOfPowerLine;
        QVector<QVector2D> pointsOfPowerLineReverse;
        pointsOfPowerLine.push_back(summaryFieldInPoint(QVector2D(event->pos())));
        
        int debugCount = 0;
        while (true) 
        {
            QVector2D tempPoint = summaryFieldInPoint(pointsOfPowerLine.back());
            if (powerLineCrossChargeOrBorder(tempPoint)) break;
            pointsOfPowerLine.push_back(tempPoint);
            debugCount++;
            if (debugCount > 10000) {drawRectangle(tempPoint.toPoint(), Qt::darkRed); break; }
        }
        pointsOfPowerLineReverse.push_back(summaryFieldInPoint(mousePosVector, true));
        debugCount = 0;
        while (true)
        {
            QVector2D tempPoint = summaryFieldInPoint(pointsOfPowerLineReverse.back(), true);
            if (powerLineCrossChargeOrBorder(tempPoint)) break;
            pointsOfPowerLineReverse.push_back(tempPoint);
            //drawRectangle(tempPoint.toPoint(), Qt::black, 8);
            debugCount++;
            if (debugCount > 10000) { drawRectangle(tempPoint.toPoint(), Qt::darkRed); break; }
        }
        drawPowerLine(mousePosVector, 10000, Qt::black, false);
        drawPowerLine(mousePosVector, 10000, Qt::black, true); // reverse line
        //for (int i = 0; i < pointsOfPowerLine.size() - 1; i++) drawLineBetween(pointsOfPowerLine[i].toPoint(), pointsOfPowerLine[i + 1].toPoint(), Qt::darkGreen);
        //for (int i = 0; i < pointsOfPowerLineReverse.size() - 1; i++) drawLineBetween(pointsOfPowerLineReverse[i].toPoint(), pointsOfPowerLineReverse[i + 1].toPoint(), Qt::darkMagenta);
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

