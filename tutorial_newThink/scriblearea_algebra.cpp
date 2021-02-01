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
            for (auto pts : sh->allPoints)
            {
                float radius = (*pts - curPoint).length();
                if (radius != 0) {
                    trueFieldValue += sh->chargePerPoint / pow(radius / koef, 2);
                    res += plusFieldInPointByPoint(curPoint, *pts, sh->chargePerPoint);
                }
                //pts += 3;
            }
            break;
        }
        case sShape::ShapeType::MOVING_POINT:
        {
            float radius = (sh->movingPos - curPoint).length();
            if (radius != 0)
            {
                if (sh->interactable)
                {
                    res += plusFieldInPointByPoint(curPoint, sh->movingPos, sh->charge);
                    trueFieldValue += sh->charge / pow(radius / koef, 2);
                }
            }
            break; }
        }

    if (reverse) res *= -1.f / res.length();
    else res *= 1.f / res.length();
    
    res += curPoint;
    return res;
}

QVector2D ScribbleArea::summaryForceInPoint(const sShape& curS, bool reverse)
{
    QVector2D res;
    //float trueForceValue = 0;
    float const koef = 1000.f; //debug, field ampf on screen
    float mulCharge = 0;
    for (auto const& sh : shapes)
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            float radius = (sh->vecNodes[0].pos - curS.movingPos).length();
            if (radius != 0) res += plusForceInPointByPoint(curS.movingPos, sh->vecNodes[0].pos, sh->charge, curS.charge);
            break; }
        case sShape::ShapeType::LINE:
        {
            for (auto const pts : sh->allPoints)
            {
                float radius = (*pts - curS.movingPos).length();
                if (radius != 0) res += plusForceInPointByPoint(curS.movingPos, *pts, sh->chargePerPoint, curS.charge);
            }
            break;
        }
        case sShape::ShapeType::MOVING_POINT:
        {
            if (sh->interactable && curS.vecNodes[0].pos != sh->vecNodes[0].pos)
            {
                float radius = (sh->movingPos - curS.movingPos).length();
                if (radius != 0) res += plusForceInPointByPoint(curS.movingPos, sh->movingPos, sh->charge, curS.charge);
            }
            break; }
        }
    //trueForceValue /= 2.f;
    if (res.length() == 0) return curS.movingPos;
    //if (reverse) res *= -trueForceValue / res.length();
    //else res *= trueForceValue / res.length();

    res += curS.movingPos;
    return res;
}

QVector2D ScribbleArea::plusFieldInPointByPoint(const QVector2D curPoint, const QVector2D chargePoint, const float charge)
{
    QVector2D radiusVector = curPoint - chargePoint;
    float radius = radiusVector.length();
    if (radius == 0 || charge == 0) return QVector2D();
    float const koef = 1000.f; 
    float field = abs(charge) / pow(radius / koef, 2);
        //field = charge / pow(radius / koef, 2);
    radiusVector = radiusVector * (field / radius);
    radiusVector /= field;

    return (charge > 0) ? radiusVector : -1 * radiusVector;
    //    if (field > 0) radiusVector *= (radius + field) / radius;
     //   else radiusVector *= -1.f * (radius - field) / radius;
    //
   // return radiusVector;
}

QVector2D ScribbleArea::plusForceInPointByPoint(const QVector2D curPoint, const QVector2D chargePoint, const float curCharge, const float chargeCharge)
{
    QVector2D radiusVector = curPoint - chargePoint;
    float radius = radiusVector.length();
    if (radius == 0 || curCharge * chargeCharge == 0) return QVector2D();
    float const koef = 1000.f;

    float force = abs(curCharge) * abs(chargeCharge);
    force = force / pow(radius / koef, 2);

    radiusVector = radiusVector * (force / radius);

    radiusVector /= 5;

    return (curCharge * chargeCharge > 0) ? radiusVector : -1 * radiusVector;

    //if (force > 0) radiusVector *= (radius + force) / radius;
    //else radiusVector *= -1 * (radius - force) / radius;
    //return radiusVector;
}

bool ScribbleArea::vecCrossChargeOrBorder(const QVector2D v, const int boundSize)
{
    if (!vectorInbound(v,boundSize)) return true;
    for (auto const& sh : shapes)
    {
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            if ((v - sh->vecNodes[0].pos).length() < boundSize) return true;
            break; }
        case sShape::ShapeType::LINE: {
            for (auto const& pts : sh->allPoints)
                if ((v - *pts).length() < boundSize) return true;
            break; }
        case sShape::ShapeType::MOVING_POINT: {
            if (sh->interactable) {
                if ((v - sh->movingPos).length() < boundSize) return true;
            }
            break; }
        }    
    }
    return false;
}

bool ScribbleArea::mChargeCrossChargeOrBorder(const sShape& s, const int boundSize)
{
    if (!vectorInbound(s.movingPos, boundSize)) return true;
    for (auto const& sh : shapes)
    {
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            if ((s.movingPos - sh->vecNodes[0].pos).length() < boundSize) return true;
            break; }
        case sShape::ShapeType::LINE: {
            for (auto const& pts : sh->allPoints)
                if ((s.movingPos - *pts).length() < boundSize) return true;
            break; }
        case sShape::ShapeType::MOVING_POINT: {
            if (sh->interactable && s.vecNodes[0].pos != sh->vecNodes[0].pos) {
                if ((s.movingPos - sh->movingPos).length() < boundSize) return true;
            }
            break; }
        }
    }
    return false;
}

void ScribbleArea::calculatePotencial()
{
    //arrayOfPotencials.clear();
    float radius;
    minPot = 0;
    avgPot = 0;
    maxPot = 0;
    /*
    for (int y = 0; y < height(); y += potScale)
    {
        //QVector<float> tempPot(width());
        for (int x = 0; x < width(); x += potScale)
        {
            //tempPot[x] = 0;
            arrayOfPotencials[y][x] = 0.f;
            for (auto const& sh : shapes) {
                switch (sh->shapeType)
                {
                case sShape::ShapeType::POINT:
                {
                    radius = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
                    //if (radius > 3) tempPot[x] += (sh->charge / radius); break;
                    if (radius > 3) arrayOfPotencials[y][x] += (sh->charge / radius); break;
                }
                case sShape::ShapeType::LINE:
                {
                    for (auto const& pts : sh->allPoints)
                    {
                        radius = QVector2D(x, y).distanceToPoint(*pts);
                        //if (radius > 3) tempPot[x] += sh->charge / (radius * sh->allPoints.size());
                        if (radius > 3) arrayOfPotencials[y][x] += sh->charge / (radius * sh->allPoints.size());
                    }
                    break;
                }
                case sShape::ShapeType::MOVING_POINT:
                {
                    if (sh->interactable)
                    {
                        radius = QVector2D(x, y).distanceToPoint(sh->movingPos);
                        ///if (radius > 3) tempPot[x] += (sh->charge / radius); break;
                        if (radius > 3) arrayOfPotencials[y][x] += (sh->charge / radius);
                    }
                    break;
                }
                }

            }
            //if (tempPot[x] > maxPot) maxPot = tempPot[x];
            //if (tempPot[x] < minPot) minPot = tempPot[x];
            //avgPot += tempPot[x];
            if (arrayOfPotencials[y][x] > maxPot) maxPot = arrayOfPotencials[y][x];
            if (arrayOfPotencials[y][x] < minPot) minPot = arrayOfPotencials[y][x];
            avgPot += arrayOfPotencials[y][x];
        }
        //arrayOfPotencials.push_back(tempPot);
    }
    */

    calculatePotencialThreadPool(100,100);
    avgPot /= (height() * width());
    potShouldReCalc = false;
}

bool ScribbleArea::clickNearShapeNode(QVector2D& mouseClick)
{
    for (auto& sh : shapes)
        for (auto& node : sh->vecNodes)
            if (mouseClick.distanceToPoint(node.pos) < 10)
            {
                node.attachedToCursor = true;
                return true;
            }
    return false;
}

bool ScribbleArea::mouseInboundArea() { return vectorInbound(mouseVector); }

bool ScribbleArea::vectorInbound(QVector2D v, int boundSize)
{

    if (v.x() > width() - boundSize ||
        v.x() < boundSize ||
        v.y() > height() - boundSize ||
        v.y() < boundSize) return false;
    return true;
}

QVector2D ScribbleArea::inboundVector(QVector2D vectorToCheck, int boundSize)
{
    if (vectorToCheck.x() < 5) vectorToCheck.setX(0.f + boundSize);
    if (vectorToCheck.x() > width()-5) vectorToCheck.setX(width() - boundSize);
    if (vectorToCheck.y() < 5) vectorToCheck.setY(0.f + boundSize);
    if (vectorToCheck.y() > height()-5) vectorToCheck.setY(height() - boundSize);
    return vectorToCheck;
}

void ScribbleArea::startSimulateMovement()
{
    int index = 0;
    for (auto& sh : shapes)
    {
        if (sh->shapeType == sShape::ShapeType::MOVING_POINT) {
            float testFloat = sh->movingPos.x();
            bool chargeSign = (sh->charge < 0) ? true : false;
            //sh->movingPos.setX(testFloat += 3);

            trajectoryMap[index].push_back(sh->movingPos);

            QVector2D tempPos = sh->movingPos;

            sh->forceVec = summaryForceInPoint(*sh);
            sh->speedVec -= tempPos;
            sh->forceVec -= tempPos;

            sh->forceVec /= 3;
            sh->speedVec = sh->speedVec + sh->forceVec;

            sh->speedVec += tempPos;
            sh->forceVec += tempPos;

            sh->movingPos = sh->speedVec - sh->movingPos;
            sh->movingPos /= 30;
            sh->movingPos += tempPos;
            sh->speedVec = sh->speedVec - tempPos + sh->movingPos;
            sh->forceVec = sh->forceVec - tempPos + sh->movingPos;

            trajectoryMap[index].push_back(sh->movingPos);
            potShouldReCalc = true;
            if (mChargeCrossChargeOrBorder(*sh, 10)) sh->interactable = false;
        }
        index++;
    }
    bool checkAlive = false;
    for (auto& sh : shapes)
        if (sh->shapeType == sShape::ShapeType::MOVING_POINT && sh->interactable) checkAlive = true;
    if (checkAlive == false) stopSimulateMovement();
    //QThread::msleep(50);
    updateShapes();
}

void ScribbleArea::stopSimulateMovement()
{
    //if (scribblemodes == SIMULATE) scribblemodes = TRACKING;
    trajectoryMap.clear();
    bool shapesNeedUpdate = false;
    for (auto& sh : shapes)
        if (sh->shapeType == sShape::ShapeType::MOVING_POINT) {
            if (sh->movingPos != sh->vecNodes[0].pos) shapesNeedUpdate = true;
            sh->movingPos = sh->vecNodes[0].pos;
            sh->interactable = true;
            sh->forceVec = sh->vecNodes[0].pos;
            sh->speedVec = sh->vecNodes[1].pos;
        }
    if (shapesNeedUpdate) updateShapes();
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