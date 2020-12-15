#pragma once
#include "vec2D.h"
#include "qvector2d.h"
#include <QVector>
#include <vector>
#include <QWidget>
#include <cmath>

class sShape;

struct sNode
{
	sShape* parent;
	QVector2D pos;
	bool attachedToCursor = { false };
	sNode() {}
	sNode(sShape* parent_, QVector2D pos_) : pos(pos_), parent(parent_) {}
};

struct sShape
{
	// Shapes are defined by the placment of nodes
	std::vector<sNode> vecNodes;
	std::list<QVector2D*> allPoints;
	QVector2D movingPos; // for moving
	uint32_t nMaxNodes = 0;

	enum ShapeType {POINT,LINE, MOVING_POINT} shapeType;
	~sShape() {
		for (auto i = allPoints.begin(); i != allPoints.end(); i++) delete* i;
	}
	
	float charge = { 1 };
	float length;
	float chargePerPoint = charge;

	// Convert coordinates from World Space --> Screen Space

	void addNode(QVector2D POS) { if (vecNodes.size() < nMaxNodes) vecNodes.push_back(sNode(this, POS));}
	QColor getColor() { return (charge > 0) ? Qt::red : Qt::blue; }

	float speed = { 0 };
	bool moveable = { false };
	bool interactable = { true };
	QVector2D forceVec, speedVec;

};

struct sLine : public sShape
{
	sLine(QVector2D p1, QVector2D p2, float charge_ = 1)
	{
		nMaxNodes = 2;
		charge = charge_ * p1.distanceToPoint(p2);
		charge = charge_; // FOR TEST
		addNode(p1);
		addNode(p2);
		shapeType = LINE;
		length = p1.distanceToPoint(p2);
		float lambda;
		for (float i = 1; i < length; i++)
		{
			lambda = i / (length - i);
			allPoints.push_back(new QVector2D((p1.x() + lambda * p2.x()) / (1 + lambda), (p1.y() + lambda * p2.y()) / (1 + lambda)));
		}
		chargePerPoint = charge_ / length;
	}
};

struct sPoint : public sShape
{
	sPoint(QVector2D p1, float charge_ = 1)
	{
		charge = charge_;
		nMaxNodes = 1;
		//.push_back(sNode(this, POS));
		addNode(p1);
		allPoints.push_back(new QVector2D(p1));
		//vecNodes.reserve(nMaxNodes);
		shapeType = POINT;
	}
};

struct sMovingPoint : public sShape
{
	sMovingPoint(QVector2D p1, QVector2D p2, float charge_ = 1)
	{
		{
			nMaxNodes = 2;
			charge = charge_; 
			addNode(p1);
			addNode(p2);
			shapeType = MOVING_POINT;
			allPoints.push_back(new QVector2D(p1));
			chargePerPoint = charge_;
			movingPos = p1;
			moveable = true;
			speedVec = p2;
			forceVec = p1;
		}
	}
};

struct StorageEqPts
{
	QPoint pos;
	float precision;
	StorageEqPts(QPoint pos_, float precision_) :pos(pos_), precision(precision_) {}
};
