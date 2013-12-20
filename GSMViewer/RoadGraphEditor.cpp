#include "RoadGraphEditor.h"
#include "GraphUtil.h"

RoadGraphEditor::RoadGraphEditor() {
	roads = new RoadGraph();
	clipBoard = new ClipBoard();

	clear();
}

RoadGraphEditor::~RoadGraphEditor() {
	clear();
}

/**
 * Clear the road graph, and free all the allocated memory as well.
 */
void RoadGraphEditor::clear() {
	roads->clear();

	mode = MODE_DEFAULT;
	selectedVertex = NULL;
	selectedEdge = NULL;
	movingVertex = NULL;
	selectedRoads = NULL;
	clipBoard->clear();

	for (int i = 0; i < history.size(); i++) {
		delete history[i];
	}
	history.clear();
}

void RoadGraphEditor::openRoad(QString filename) {
	clear();

	GraphUtil::loadRoads(roads, filename, 7);
}

void RoadGraphEditor::saveRoad(QString filename) {
	GraphUtil::saveRoads(roads, filename);
}

void RoadGraphEditor::undo() {
	if (history.size() == 0) return;

	GraphUtil::copyRoads(history[history.size() - 1], roads);
	history.pop_back();
	roads->setModified();

	selectedVertex = NULL;
	selectedEdge = NULL;
}

void RoadGraphEditor::cut() {
	if (mode != MODE_AREA_SELECTED) return;

	history.push_back(GraphUtil::copyRoads(roads));

	// extract the roads within the area, and put it into the clipboard.
	clipBoard->copy(roads, bbox);

	GraphUtil::subtractRoads(roads, bbox, true);
}

void RoadGraphEditor::copy() {
	if (mode != MODE_AREA_SELECTED) return;

	// extract the roads within the area, and put it into the clipboard.
	clipBoard->copy(roads, bbox);
}

void RoadGraphEditor::paste() {
	if (clipBoard->empty()) return;

	if (selectedRoads != NULL) {
		delete selectedRoads;
	}
	selectedRoads = clipBoard->paste();

	// update the bbox according to the paseted roads
	bbox = GraphUtil::getAABoundingBox(selectedRoads);

	// move the pasted roads to the center of the screen (to be updated!!!)
	GraphUtil::translate(selectedRoads, bbox.minPt * -1.0f);
	bbox = GraphUtil::getAABoundingBox(selectedRoads);

	// inflate the bbox a little so that all the vertices are completely within the bbox.
	bbox.minPt.setX(bbox.minPt.x() - bbox.dx() * 0.1f);
	bbox.minPt.setY(bbox.minPt.y() - bbox.dy() * 0.1f);
	bbox.maxPt.setX(bbox.maxPt.x() + bbox.dx() * 0.1f);
	bbox.maxPt.setY(bbox.maxPt.y() + bbox.dy() * 0.1f);

	mode = MODE_AREA_SELECTED;
}

bool RoadGraphEditor::deleteEdge() {
	if (selectedEdge == NULL) return false;

	history.push_back(GraphUtil::copyRoads(roads));
	roads->graph[selectedEdgeDesc]->valid = false;
	selectedEdge = NULL;
	roads->setModified();

	return true;
}

void RoadGraphEditor::simplify(float threshold) {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::simplify(roads, threshold);
}

void RoadGraphEditor::reduce() {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::reduce(roads);
}

void RoadGraphEditor::removeShortDeadend(float threshold) {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::removeShortDeadend(roads, threshold);
}

void RoadGraphEditor::selectAll() {
	bbox = GraphUtil::getAABoundingBox(roads);
}

void RoadGraphEditor::startArea(const QVector2D& pt) {
	bbox.reset();
	bbox.addPoint(pt);

	mode = MODE_DEFINING_AREA;
}

void RoadGraphEditor::updateArea(const QVector2D& pt) {
	bbox.maxPt.setX(pt.x());
	bbox.maxPt.setY(pt.y());
}

void RoadGraphEditor::finalizeArea() {
	if (bbox.maxPt.x() < bbox.minPt.x()) {
		float x = bbox.maxPt.x();
		bbox.maxPt.setX(bbox.minPt.x());
		bbox.minPt.setX(x);
	}
	if (bbox.maxPt.y() < bbox.minPt.y()) {
		float y = bbox.maxPt.y();
		bbox.maxPt.setY(bbox.minPt.y());
		bbox.minPt.setY(y);
	}

	// if the box is just a single point, cancel the selection.
	if (bbox.maxPt.x() == bbox.minPt.x() && bbox.maxPt.y() == bbox.minPt.y()) {
		mode = MODE_DEFAULT;
	} else {
		history.push_back(GraphUtil::copyRoads(roads));

		// copy the roads in the area to "selectedRoads"
		if (selectedRoads != NULL) {
			delete selectedRoads;
		}
		selectedRoads = GraphUtil::copyRoads(roads);
		GraphUtil::extractRoads(selectedRoads, bbox, true);
		GraphUtil::clean(selectedRoads);

		// subtract the area from the roads
		GraphUtil::subtractRoads(roads, bbox, true);

		mode = MODE_AREA_SELECTED;
	}
}

void RoadGraphEditor::moveArea(float dx, float dy) {
	if (mode != MODE_AREA_SELECTED) return;

	bbox.minPt.setX(bbox.minPt.x() + dx);
	bbox.minPt.setY(bbox.minPt.y() + dy);
	bbox.maxPt.setX(bbox.maxPt.x() + dx);
	bbox.maxPt.setY(bbox.maxPt.y() + dy);

	if (selectedRoads != NULL) {
		GraphUtil::translate(selectedRoads, QVector2D(dx, dy));
	}
}

bool RoadGraphEditor::selectVertex(const QVector2D& pt) {
	if (roads == NULL) return false;

	RoadVertexDesc desc;
	if (!GraphUtil::getVertex(roads, pt, roads->widthBase, desc)) {
		selectedVertex = NULL;
		return false;
	}

	selectedVertexDesc = desc;
	selectedVertex = roads->graph[desc];

	// clear the selction for other items
	selectedEdge = NULL;

	// update the mode
	mode = MODE_VERTEX_SELECTED;

	return true;
}

bool RoadGraphEditor::selectEdge(const QVector2D& pt) {
	if (roads == NULL) return false;

	RoadEdgeDesc desc;
	if (!GraphUtil::getEdge(roads, pt, roads->widthBase, desc)) {
		selectedEdge = NULL;
		return false;
	}

	selectedEdgeDesc = desc;
	selectedEdge = roads->graph[selectedEdgeDesc];

	// clear the selction for other items
	selectedVertex = NULL;

	mode = MODE_EDGE_SELECTED;

	return true;
}

void RoadGraphEditor::moveSelectedVertex(const QVector2D& pt) {
	if (roads == NULL) return;

	if (movingVertex == NULL) {
		history.push_back(GraphUtil::copyRoads(roads));
		movingVertex = selectedVertex;
	}
	
	GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
}

void RoadGraphEditor::moveSelectedVertex(const QVector2D& pt, float snap_threshold) {
	if (roads == NULL) return;

	if (movingVertex == NULL) {
		history.push_back(GraphUtil::copyRoads(roads));
		movingVertex = selectedVertex;
	}
	
	// if there is a vertex close to this point, snap to it.
	RoadVertexDesc desc;
	if (GraphUtil::getVertex(roads, pt, snap_threshold, selectedVertexDesc, desc)) {
		GraphUtil::moveVertex(roads, selectedVertexDesc, roads->graph[desc]->pt);
	} else {
		GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
	}
}

void RoadGraphEditor::stopMovingSelectedVertex() {
	movingVertex = NULL;
}

void RoadGraphEditor::stopMovingSelectedVertex(float snap_threshold) {
	if (movingVertex != NULL) {
		// if there is a vertex close to this point, snap to it.
		RoadVertexDesc desc;
		if (GraphUtil::getVertex(roads, roads->graph[selectedVertexDesc]->pt, snap_threshold, selectedVertexDesc, desc)) {
			GraphUtil::snapVertex(roads, selectedVertexDesc, desc);
		}
	}

	movingVertex = NULL;
}

/**
 * Merge the selected roads to the actual roads.
 */
void RoadGraphEditor::unselectRoads() {
	if (selectedRoads != NULL) {
		GraphUtil::mergeRoads(roads, selectedRoads);
		delete selectedRoads;
		selectedRoads = NULL;
	}

	mode = MODE_DEFAULT;
}

/**
 * Merge the selected roads to the actual roads. 
 * By tring to connect the selected roads to the other as much as possible, the resulting roads hopefully look natural.
 */
void RoadGraphEditor::connectRoads() {
	if (selectedRoads != NULL) {
		GraphUtil::connectRoads(roads, selectedRoads, 60.0f);
		delete selectedRoads;
		selectedRoads = NULL;
	}

	mode = MODE_DEFAULT;
}

bool RoadGraphEditor::splitEdge(const QVector2D& pt) {
	RoadEdgeDesc e_desc;
	if (GraphUtil::getEdge(roads, pt, roads->widthBase, e_desc)) {
		GraphUtil::splitEdge(roads, e_desc, pt);
		return true;
	} else {
		return false;
	}
}
