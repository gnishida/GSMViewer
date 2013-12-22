#pragma once

#include "AbstractArea.h"
#include "RoadGraph.h"

class ClipBoard {
public:
	RoadGraph* roads;

public:
	ClipBoard();
	~ClipBoard();

	void clear();
	bool empty() const;
	void copy(RoadGraph* roads);
	void copy(RoadGraph* roads, const AbstractArea& area);
	RoadGraph* paste();
};

