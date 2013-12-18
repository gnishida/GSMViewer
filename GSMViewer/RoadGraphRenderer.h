#pragma once

#include "RoadGraph.h"
#include "Renderable.h"

class RoadGraphRenderer {
public:
	unsigned int dispList;

public:
	RoadGraphRenderer();

	void render(Renderable* renderable);
};

