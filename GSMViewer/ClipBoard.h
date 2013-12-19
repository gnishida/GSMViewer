#pragma once

#include "BBox.h"
#include "RoadGraph.h"

class ClipBoard {
public:
	BBox bbox;
	RoadGraph* roads;

public:
	ClipBoard(const BBox& bbox, RoadGraph* roads);
	~ClipBoard();
};

