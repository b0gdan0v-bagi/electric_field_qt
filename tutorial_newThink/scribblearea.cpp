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
    drawRectangle(QPoint(this->size().width(), this->size().height()));
    drawRectangle(QPoint(10, 10));
    this->setBaseSize(500, 500);
    this->size().setWidth(10);
    this->size().setHeight(10);
    //shapes.push_back(new sPoint(QPoint(this->size().width() / 2, this->size().height() / 2)));
    //for (int i=0;i<width();i++)
    //    for (int j=0;j<height();j++)
            //shapes.push_back(new sPoint(QPoint(i*50 , j*50 )));
    
    //QColor(Qrbg 2);
    int testx_1 = 100 + width() / 4;
    int testx_2 = 200 + 3*width() / 4;
    int testy_1 = 100+10*height() / 2;
    int testy_2 = 100 + 10*height() / 2;
    shapes.push_back(new sPoint(QPoint(testx_1, testy_1)));
    shapes.push_back(new sPoint(QPoint(testx_2, testy_2), -1.f));
    

    updateShapes();
}

// Used to load the image and place it in the widget
bool ScribbleArea::openImage(const QString& fileName)
{
    // Holds the image
    QImage loadedImage;

    // If the image wasn't loaded leave this function
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

// Save the current image
bool ScribbleArea::saveImage(const QString& fileName, const char* fileFormat)
{
    // Created to hold the image
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    }
    else {
        return false;
    }
}


// Color the image area with white
void ScribbleArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void ScribbleArea::updateShapes()
{
    clearImage();
    for (auto const& sh : shapes)
    {
        switch (sh->shapeType)
        {
        case 0: {drawRectangle(sh->vecNodes[0].pos); break; }
        case 1: {
            for (int i = 0; i < sh->vecNodes.size(); i++) drawRectangle(sh->vecNodes.at(i).pos);
            // "if" for checking only
            if (sh->vecNodes.size() > 1) drawLineBetween(sh->vecNodes.at(0).pos, sh->vecNodes.at(1).pos);
            break; }
        }
    }
}

void ScribbleArea::calculatePotencial()
{
    //arrayOfPotencials.clear();
    float radius;
    float minPot = 0;
    for (auto const& i : shapes)
        minPot += (1 / std::sqrt(pow(i->vecNodes[0].pos.x(), 2) + pow(i->vecNodes[0].pos.y(), 2)));
    float maxPot = minPot;

    for (int y = 0; y < height(); y++)
    {
        std::vector<float> tempPot(width());
        for (int x = 0; x < width(); x++)
        {
            tempPot[x] = 0;
            for (auto const& i : shapes) {
                radius = std::sqrt(pow(x - i->vecNodes[0].pos.x(), 2) + pow(y - i->vecNodes[0].pos.y(), 2));
                if (radius!=0) tempPot[x] += (i->charge / radius);
            }
            if (tempPot[x] > maxPot) maxPot = tempPot[x];
            if (tempPot[x] < minPot) minPot = tempPot[x];
        }
        arrayOfPotencials.push_back(tempPot);
    }
    QPainter painter(&image);
    //QMessageBox akaDebug;
    //akaDebug.setText("min = " + QString::number(minPot) + " max = " + QString::number(maxPot));
    //akaDebug.exec();
    

    //float const maxV = (float)pow(256, 3);
    float const maxV = 200;
    for (int y = 0; y < height(); y++)
    {
        for (int x = 0; x < width(); x++)
        {
            painter.setPen(QPen(QColor::fromRgb(200,
                 maxV * (arrayOfPotencials[y][x] - minPot) / (maxPot - minPot),
                255),
                1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPoint(x, y);
        }
    }
    update();   
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
    if (findEqvivalent)
    {
        if (event->button() == Qt::LeftButton && findEqvivalent) calcEqPot(event->pos());
    }
    else { // test
    


        if (event->button() == Qt::LeftButton && creatingShape)
        {
            switch (scribblemodes)
            {
            case ScribbleArea::NONE:
            {
                lastPoint = event->pos();
                scribbling = true;
                //creatingShape = false;
                break;
            }
            case ScribbleArea::LINE:
            {
                switch (nNodes) {
                case 2: {
                    point1 = event->pos(); drawRectangle(point1); nNodes--;
                    scribbling = true;
                    setMouseTracking(true);
                    break; }
                case 1: {
                    point2 = event->pos();
                    //drawRectangle(point2); 
                    nNodes++;
                    shapes.push_back(new sLine(point1, point2));
                    scribbling = false;
                    setMouseTracking(false);
                    updateShapes();
                    //drawLineBetween(point1, point2);
                    //creatingShape = false;
                    break; }
                }
                break;
            }
            case ScribbleArea::SINGLE: {
                //drawRectangle(event->pos());
                shapes.push_back(new sPoint(event->pos()));
                updateShapes();
                break; }
            case ScribbleArea::CYLINDER: break;

            }
            emit dataReady(shapes);
        }
    }
}

// When the mouse moves if the left button is clicked
// we call the drawline function which draws a line
// from the last position to the current
void ScribbleArea::mouseMoveEvent(QMouseEvent* event)
{
    if (nNodes == 1 && scribblemodes == ScribbleModes::LINE)
    {
        updateShapes();
        drawRectangle(point1);
        drawLineBetween(point1, event->pos());
        drawRectangle(event->pos());

    }
    
    //if (event->buttons()) {  }
    //if (event->buttons() == Qt::RightButton) drawRectangle(event->pos());
    if (scribblemodes == NONE)
        //if ((event->buttons() & Qt::LeftButton) && scribbling) drawLineTo(event->pos());
    if (scribblemodes == LINE && nNodes == 1) {
       // if ((event->buttons() & Qt::LeftButton) && scribbling) drawLineTo(event->pos());
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

