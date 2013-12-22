#include "ClipBoard.h"
#include "GraphUtil.h"

ClipBoard::ClipBoard() {
	roads = NULL;
}

ClipBoard::~ClipBoard() {
}

void ClipBoard::clear() {
	if (roads != NULL) {
		delete roads;
	}

	roads = NULL;
}

bool ClipBoard::empty() const {
	return (roads == NULL) ? true : false;
}

void ClipBoard::copy(RoadGraph* roads) {
	clear();

	this->roads = GraphUtil::copyRoads(roads);
}

void ClipBoard::copy(RoadGraph* roads, const AbstractArea& area) {
	clear();

	this->roads = GraphUtil::copyRoads(roads);
	GraphUtil::extractRoads(this->roads, area, true);
	GraphUtil::clean(this->roads);
}

RoadGraph* ClipBoard::paste() {
	return GraphUtil::copyRoads(roads);
}
