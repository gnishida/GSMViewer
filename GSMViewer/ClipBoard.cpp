#include "ClipBoard.h"

ClipBoard::ClipBoard(const BBox& bbox, RoadGraph* roads) {
	this->bbox = bbox;
	this->roads = roads;
}

ClipBoard::~ClipBoard() {
}
