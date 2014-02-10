#pragma once

#include <vector>
#include <common/RoadGraph.h>
#include "AbstractForest.h"

class BFSForest : public AbstractForest {
public:
	BFSForest(RoadGraph* roads, QList<RoadVertexDesc> roots, int maxDepth = 0);
	~BFSForest();
	
	void buildForest(int maxDepth = 0);
};

