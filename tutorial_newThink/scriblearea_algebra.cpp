#include "scribblearea.h"

QVector2D ScribbleArea::findIntersectLineNormal(QVector2D line_p1, QVector2D line_p2, QVector2D outer_p)
{
    //equations of line in Ax+By+C=0 form
    float A1 = (line_p1.y() - line_p2.y());
    float B1 = (line_p2.x() - line_p1.x());
    float C1 = (line_p1.x() * line_p2.y() - line_p2.x() * line_p1.y());
    float A2 = -B1;
    float B2 = A1;
    float C2 = B1 * outer_p.x() - A1 * outer_p.y();
    float det = A1 * B2 - B1 * A2; //determenant
    if (det == 0) return QVector2D(0, 0); //error!!
    else return QVector2D((-C1 * B2 + B1 * C2) / det, (A2 * C1 - A1 * C2) / det); //inverse matrix * vector {-C1,-C2}
}

QVector2D ScribbleArea::summaryFieldInPoint(const QVector2D curPoint, bool reverse)
{
    QVector2D res;
    float trueFieldValue = 0;
    float const koef = 1000.f; //debug, field ampf on screen
    for (auto const& sh : shapes)
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            float radius = (sh->vecNodes[0].pos - curPoint).length();
            if (radius != 0)
            {
                res += plusFieldInPointByPoint(curPoint, sh->vecNodes[0].pos, sh->charge);
                trueFieldValue += sh->charge / pow(radius / koef, 2);
            }
            break; }
        case sShape::ShapeType::LINE:
        {
            for (auto const pts : sh->allPoints)
            {
                float radius = (*pts - curPoint).length();
                if (radius != 0) {
                    trueFieldValue += sh->chargePerPoint / pow(radius / koef, 2);
                    res += plusFieldInPointByPoint(curPoint, *pts, sh->chargePerPoint);
                }
            }
            break;
        }
        }
    if (reverse) res *= -1.f / res.length();
    else res *= 1.f / res.length();
    
    res += curPoint;
    return res;
}

QVector2D ScribbleArea::plusFieldInPointByPoint(const QVector2D curPoint, const QVector2D chargePoint, const float charge)
{
    QVector2D radiusVector = curPoint - chargePoint;
    float radius = radiusVector.length();
    float const koef = 1000.f; 
    float field = 0;
    if (radius != 0)
    {
        field = charge / pow(radius / koef, 2);
        if (field > 0) radiusVector *= (radius + field) / radius;
        else radiusVector *= -1 * (radius - field) / radius;
    }
    return radiusVector;
}

bool ScribbleArea::powerLineCrossChargeOrBorder(const QVector2D lastPoint)
{
    if (lastPoint.x() > 624 || lastPoint.x() < 1 || lastPoint.y() > 669 || lastPoint.y() < 1) return false;
    for (auto const& sh : shapes)
    {
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            if ((lastPoint - sh->vecNodes[0].pos).length() < 10) return true;
            break; }
        case sShape::ShapeType::LINE: {
            for (auto const& pts : sh->allPoints)
                if ((lastPoint - *pts).length() < 10) return true;
            break; }
        }    
    }
    return false;
}

void ScribbleArea::calculatePotencial()
{
    arrayOfPotencials.clear();
    float radius;
    minPot = 0;
    avgPot = 0;
    maxPot = 0;
    for (int y = 0; y < height(); y += potScale)
    {
        std::vector<float> tempPot(width());
        for (int x = 0; x < width(); x += potScale)
        {
            tempPot[x] = 0;
            for (auto const& sh : shapes) {
                switch (sh->shapeType)
                {
                case sShape::ShapeType::POINT:
                {
                    radius = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
                    if (radius > 3) tempPot[x] += (sh->charge / radius); break;
                }
                case sShape::ShapeType::LINE:
                {
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
    avgPot /= (height() * width());
    potShouldReCalc = false;
}

//drawRectangle(intersectPoint.toPoint());
//drawLineBetween(QPoint(x,y),intersectPoint.toPoint());

/*
radius = QVector2D(x, y).distanceToLine(sh->vecNodes[0].pos, sh->vecNodes[1].pos);
if (radius > 3)
{
    float l1, l2;
    float d1 = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
    float d2 = QVector2D(x, y).distanceToPoint(sh->vecNodes[1].pos);
    float l = (sh->vecNodes[1].pos - sh->vecNodes[0].pos).length();
    float eps = 0.001f;

    l1 = intersectPoint.distanceToPoint(sh->vecNodes[0].pos);
    l2 = intersectPoint.distanceToPoint(sh->vecNodes[1].pos);

    if (radius <= d1 && radius <= d2)
    {
        if (l1 != 0) tempPot[x] += (sh->charge / (2 * l1)) * log((l1 + sqrt(l1 * l1 + radius * radius)) / (-l1 + sqrt(l1 * l1 + radius * radius)));
        if (l2 != 0) tempPot[x] += (sh->charge / (2 * l2)) * log((l2 + sqrt(l2 * l2 + radius * radius)) / (-l2 + sqrt(l2 * l2 + radius * radius)));
    }
    else
    {
        if (d1 < d2)
        {
            l2 += l1;
            if (l1 != 0) tempPot[x] -= (sh->charge / (2 * l1)) * log((l1 + sqrt(l1 * l1 + radius * radius)) / (-l1 + sqrt(l1 * l1 + radius * radius)));
            if (l2 != 0) tempPot[x] += (sh->charge / (2 * l2)) * log((l2 + sqrt(l2 * l2 + radius * radius)) / (-l2 + sqrt(l2 * l2 + radius * radius)));
        }
        else
        {
            l1 += l2;
            if (l1 != 0) tempPot[x] += (sh->charge / (2 * l1)) * log((l1 + sqrt(l1 * l1 + radius * radius)) / (-l1 + sqrt(l1 * l1 + radius * radius)));
            if (l2 != 0) tempPot[x] -= (sh->charge / (2 * l2)) * log((l2 + sqrt(l2 * l2 + radius * radius)) / (-l2 + sqrt(l2 * l2 + radius * radius)));
        }
    }
    akaDebug.setText("l1 " + QString::number(l1) + " l2 " + QString::number(l2) + " l " + QString::number(l) + " l2-l1 "
        + QString::number(l2 - l1 - l) + " pot " + QString::number(tempPot[x]));
    //akaDebug.exec();    

    //tempPot[x] += sh->charge * 1 * log((l1 + sqrt(radius * radius + l1 * l1)) / (-l2 + sqrt(radius * radius + l2 * l2))) / l;
    //akaDebug.setText("l1 " + QString::number(l1)+ " l2 " + QString::number(l2) + " d1 " + QString::number(d1) + " d2 " + QString::number(d2));
    //akaDebug.exec();
}
*/