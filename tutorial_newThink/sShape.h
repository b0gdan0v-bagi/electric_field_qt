#pragma once
#include "vec2D.h"
#include <QVector>
#include <vector>
#include <QWidget>
#include <cmath>

class sShape;


struct sNode
{
	sShape* parent;
	//vf2D pos;
	QPoint pos;
	sNode() {}
	sNode(sShape* parent_, QPoint pos_) : pos(pos_), parent(parent_) {}
};

struct sShape
{
	// Shapes are defined by the placment of nodes
	std::vector<sNode> vecNodes;
	uint32_t nMaxNodes = 0;

	enum ShapeType {POINT,LINE} shapeType;

	// The colour of the shape
	//olc::Pixel col = olc::GREEN;

	// All shapes share word to screen transformation
	// coefficients, so share them staically
	static float fWorldScale;
	static vf2D vWorldOffset;
	float charge = { 1 };

	// Convert coordinates from World Space --> Screen Space
	void WorldToScreen(const vf2D& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldScale);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldScale);
	}
	void addNode(QPoint POS) { if (vecNodes.size() < nMaxNodes) vecNodes.push_back(sNode(this, POS));}

	// This is a PURE function, which makes this class abstract. A sub-class
	// of this class must provide an implementation of this function by
	// overriding it
	//virtual void DrawYourself(olc::PixelGameEngine* pge) = 0;

	// Shapes are defined by nodes, the shape is responsible
	// for issuing nodes that get placed by the user. The shape may
	// change depending on how many nodes have been placed. Once the
	// maximum number of nodes for a shape have been placed, it returns
	// nullptr
	/*sNode* GetNextNode(const vf2D& p)
	{
		if (vecNodes.size() == nMaxNodes)
			return nullptr; // Shape is complete so no new nodes to be issued

		// else create new node and add to shapes node vector
		sNode n;
		n.parent = this;
		n.pos = p;
		vecNodes.push_back(n);

		// Beware! - This normally is bad! But see sub classes
		return &vecNodes[vecNodes.size() - 1];
	}

	// Test to see if supplied coordinate exists at same location
	// as any of the nodes for this shape. Return a pointer to that
	// node if it does
	sNode* HitNode(vf2D& p)
	{
		for (auto& n : vecNodes)
		{
			if ((p - n.pos).mag() < 0.01f)
				return &n;
		}

		return nullptr;
	}*/
	//virtual void drawToScribbleArea() = 0;


	// Draw all of the nodes that define this shape so far
	/*void DrawNodes(olc::PixelGameEngine* pge)
	{
		for (auto& n : vecNodes)
		{
			int sx, sy;
			WorldToScreen(n.pos, sx, sy);
			pge->FillCircle(sx, sy, 2, olc::RED);
		}
	}*/
};

struct sLine : public sShape
{
	sLine(QPoint p1, QPoint p2)
	{
		nMaxNodes = 2;
		//vecNodes.reserve(nMaxNodes); // We're gonna be getting pointers to vector elements
		// though we have defined already how much capacity our vector will have. This makes
		// it safe to do this as we know the vector will not be maniupulated as we add nodes
		// to it. Is this bad practice? Possibly, but as with all thing programming, if you
		// know what you are doing, it's ok :D

		addNode(p1);
		addNode(p2);
		shapeType = LINE;
	}
	void drawToScribbleArea()
	{

	}
	// Implements custom DrawYourself Function, meaning the shape
	// is no longer abstract
	/*void DrawYourself(olc::PixelGameEngine* pge) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
		pge->DrawLine(sx, sy, ex, ey, col);
	}*/
};

struct sPoint : public sShape
{
	sPoint(QPoint POS, float charge_ = 1)
	{
		charge = charge_;
		nMaxNodes = 1;
		//.push_back(sNode(this, POS));
		addNode(POS);
		//vecNodes.reserve(nMaxNodes);
		shapeType = POINT;
	}
};

