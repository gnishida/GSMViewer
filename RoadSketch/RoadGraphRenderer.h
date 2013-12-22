#pragma once

#include "RoadGraph.h"
#include "Renderable.h"
#include "BBox.h"

class RoadGraphRenderer {
public:
	unsigned int dispList;

public:
	RoadGraphRenderer();

	void render(std::vector<Renderable>& renderables);
	void renderGeometry(Renderable* renderable);

	void renderArea(const AbstractArea& area, float height);
	void renderPoint(const QVector2D& pt, float height);
	void renderPolyline(std::vector<QVector2D>& polyline, float height);
};

