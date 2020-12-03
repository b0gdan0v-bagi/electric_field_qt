/*QVector2D intersectPoint;
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
}*/
