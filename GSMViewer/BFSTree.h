#pragma once

#include <vector>
#include <common/RoadGraph.h>
#include "AbstractForest.h"

class BFSTree : public AbstractForest {
public:
	int depth;

public:
	BFSTree(RoadGraph* roads, RoadVertexDesc root, int maxDepth = 0);
	~BFSTree();
	
	void buildForest(int maxDepth = 0);
};

