#include "scribblearea.h"

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

}

void ScribbleArea::drawArrow(const QVector2D& fromPoint, const QVector2D& toPoint, float head_length, const float head_width, const QColor lineColor)
{
    const float dx = toPoint.x() - fromPoint.x();
    const float dy = toPoint.y() - fromPoint.y();
    const auto length = std::sqrt(dx * dx + dy * dy);
    if (head_length < 1 || length < head_length) return;

    // ux,uy is a unit vector parallel to the line.
    const auto ux = dx / length;
    const auto uy = dy / length;

    // vx,vy is a unit vector perpendicular to ux,uy
    const auto vx = -uy;
    const auto vy = ux;

    const auto half_width = 0.5f * head_width;

    QPoint arrowEnd_1(toPoint.x() - head_length * ux + half_width * vx, toPoint.y() - head_length * uy + half_width * vy);
    QPoint arrowEnd_2(toPoint.x() - head_length * ux - half_width * vx, toPoint.y() - head_length * uy - half_width * vy);
    QPainter painter(&image);
    painter.setPen(QPen(lineColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    painter.drawLine(fromPoint.toPoint(), toPoint.toPoint());
    painter.drawLine(arrowEnd_1, toPoint.toPoint());
    painter.drawLine(arrowEnd_2, toPoint.toPoint());
    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update(QRect(fromPoint.toPoint(), toPoint.toPoint()).normalized()
        .adjusted(-rad, -rad, +rad, +rad));
}

void ScribbleArea::calcEqPot(QPoint& point)
{
    QPainter painter(&image);
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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

void ScribbleArea::drawPowerLine(QVector2D& startPos, const int maxPts, const QColor& lineColor, const bool reverse)
{
    QVector<QVector2D> pointsOfPowerLine;
    pointsOfPowerLine.push_back(summaryFieldInPoint(startPos, reverse));
    int ptsCount = 0;
    while (true) {
        QVector2D tempPoint = summaryFieldInPoint(pointsOfPowerLine.back(), reverse);
        if (powerLineCrossChargeOrBorder(tempPoint)) break;
        pointsOfPowerLine.push_back(tempPoint);
        ptsCount++;
        if (ptsCount >= maxPts) break;

    }
    drawLines(pointsOfPowerLine, lineColor);
    if (!reverse)
        if (pointsOfPowerLine.size() > 15)  drawArrow(pointsOfPowerLine[pointsOfPowerLine.size()-12], pointsOfPowerLine.back()); 
    //for (QVector<QVector2D*>::iterator pts = pointsOfPowerLine.begin();pts!=pointsOfPowerLine.end();pts++)
        //delete* pts;
}

void ScribbleArea::drawLines(QVector<QVector2D>& pointsToDraw, const QColor &lineColor) 
{
    QVector<QLineF> linesToDraw;
    for (int i = 0; i < pointsToDraw.size() - 1; i++) linesToDraw.push_back(QLineF(pointsToDraw[i].toPoint(), pointsToDraw[i + 1].toPoint()));
    QPainter painter(&image);
    // Set the current settings for the pen
    painter.setPen(QPen(lineColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
        Qt::RoundJoin));
    painter.drawLines(linesToDraw);
    update();
}


void ScribbleArea::drawElFieldAllArea()
{
    for (int y = 10; y < height(); y+=40)
        for (int x = 10; x < width(); x+=40)
        {
            drawPowerLine(QVector2D(x, y), 30);
        }

}