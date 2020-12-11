#include "scribblearea.h"


ScribbleArea::ScribbleArea(QWidget* parent)
    : QWidget(parent), QObject(parent)
{
    // Roots the widget to the top left even if resized
    setAttribute(Qt::WA_StaticContents);

    // Set defaults for the monitored variables
    modified = false;
    scribbling = false;
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
    if (potShouldReCalc && drawPotMap) calculatePotencial();
    if (!potShouldReCalc && drawPotMap) drawPotencialAllArea();
    if (drawEqPotLines)
    {
        calcEqPot(mousePoint);
        for (auto &pts : storageEqPts) calcEqPot(pts);
    }

    if (drawPowerLines) drawAllPowerLines();
        
    if (drawElField) drawElFieldAllArea();
}


// If a mouse button is pressed check if it was the
// left button and if so store the current position
// Set that we are currently drawing
void ScribbleArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Make sure user we destroy line if user want to switch another thing while LINE havent finished
        if (scribblemodes != LINE) nNodes = 2; 
        //if (storePtsEqPotLines) 
            //if (event->pos().x() < width() && event->pos().x() > 1 && event->pos().y() < height() && event->pos().y() > 1)
                //storageEqPts.push_back(event->pos());

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
                potShouldReCalc = true;
               // for (auto const& pts : shapes.back()->allPoints)
               //     drawRectangle(pts->toPoint());
                scribbling = false;
                //setMouseTracking(false);
                updateShapes();
            }
            break; 
        }
        case ScribbleArea::POINT: {
            shapes.push_back(new sPoint(QVector2D(event->pos()),chargeToAdd));
            potShouldReCalc = true;
            updateShapes();
            break; }
        case ScribbleArea::CYLINDER:
            break;
        case ScribbleArea::EQPOTLINES: {storageEqPts.push_back(event->pos());/* calcEqPot(event->pos())*/; break; }
        case ScribbleArea::TRACKING: {
            if (clickNearShapeNode(QVector2D(event->pos()))) nodeHited = true;

            break;
        }
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
    //drawText(QPoint(50, 50), "width = " + QString::number(width()) + "\nheight = " + QString::number(height()));
    drawText(QPoint(50, 50), "N = " + QString::number(scribblemodes));
    mousePoint = event->pos();

    
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
    case ScribbleArea::POINT: {updateShapes(); drawRectangle(event->pos()); break; }
    case ScribbleArea::CYLINDER:
        break;
    case ScribbleArea::EQPOTLINES: { storePtsEqPotLines = true; updateShapes(); break; }
    case ScribbleArea::DIRECTIONS: break;
    case ScribbleArea::TRACKING: {
        if (nodeHited)
        {
            
        }

        setMouseTracking(true);
        QString textToShow;
        float const koef = 1000.f; //debug, field ampf on screen
        float fieldValue = 0;
        int shapeNumber = 1;
        for (auto const& sh : shapes)
        {
            switch (sh->shapeType)
            {
            case sShape::ShapeType::POINT: {
                if (QVector2D(event->pos()).distanceToPoint(sh->vecNodes[0].pos) < 10)
                    textToShow += "Point number " + QString::number(shapeNumber) + " "
                        + "\npos: " + QString::number(sh->vecNodes[0].pos.x()) + ", " + QString::number(sh->vecNodes[0].pos.y())
                        + "\ncharge: " + QString::number(sh->charge) + "\n";
                float radius = (sh->vecNodes[0].pos - QVector2D(event->pos())).length();
                if (radius != 0) fieldValue += sh->charge / pow(radius / koef, 2);

                break; }
            case sShape::ShapeType::LINE:
            {

                bool lineIsNear = false;
                for (auto const pts : sh->allPoints)
                {
                    if (QVector2D(event->pos()).distanceToPoint(*pts) < 10) lineIsNear = true;
                    float radius = (*pts - QVector2D(event->pos())).length();
                    if (radius != 0) fieldValue += sh->chargePerPoint / pow(radius / koef, 2);
                }
                if (lineIsNear)
                    textToShow += "Line number " + QString::number(shapeNumber) + " "
                    + "\npos: " + QString::number(sh->vecNodes[0].pos.x()) + ", " + QString::number(sh->vecNodes[0].pos.y())
                    + "\npos: " + QString::number(sh->vecNodes[1].pos.x()) + ", " + QString::number(sh->vecNodes[1].pos.y())
                    + "\ncharge: " + QString::number(sh->charge) + "\n";
                break;
            }
            }
            shapeNumber++;
        }
        if (potShouldReCalc) calculatePotencial();
        textToShow += "potencial = " + QString::number(arrayOfPotencials[event->pos().y()][event->pos().x()]) + "\n";
        textToShow += "Field value = " + QString::number(fieldValue) + "\n";
        QToolTip::showText(event->globalPos(),textToShow, this, rect());

        break;
    }
    }
    storePtsEqPotLines = false;
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

