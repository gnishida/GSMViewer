#pragma once

#include "RoadGraph.h"
#include "BFSTree.h"
#include "ShadowRoadGraph.h"
#include <QMap>
#include <QList>

class RoadGraphDatabase {
public:
	RoadGraph* roads;
	QMap<RoadVertexDesc, BFSTree*> trees;

public:
	RoadGraphDatabase();
	~RoadGraphDatabase();

	void load(QString filename);
	void findSimilarRoads(RoadGraph* roads1, int N, QList<ShadowRoadGraph*>& results);

private:
	void createTrees(RoadGraph* roads);
	void clearTrees();
	float computeRotationAngle(RoadGraph* roads1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map);
};

