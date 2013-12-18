#pragma once

#include <qvector3d.h>
#include <vector>

class RoadEdge {
public:
	bool oneWay;
	int lanes;
	int type;
	std::vector<QVector2D> polyLine;
	float weight;
	float importance;

	bool valid;			// if this edge is valid
	bool seed;			// if this edge is used as a seed of a forest
	int group;			// to which tree in the forest this edge belongs to
	bool fullyPaired;	// if this edge has a corresponding edge

public:
	RoadEdge(unsigned int lanes, unsigned int type, bool oneWay);
	~RoadEdge();
	
	float getLength();
	std::vector<QVector2D> getPolyLine();

	void addPoint(const QVector2D &pt);
	float getWidth(float widthPerLane);

	bool containsPoint(const QVector2D &pos, float widthPerLane, int& index);
};

