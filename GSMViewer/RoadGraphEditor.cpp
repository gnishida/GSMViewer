#include "RoadGraphEditor.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include "ArcArea.h"

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
	selectedRoadsOrig = NULL;
	selectedArea = NULL;
	clearInterpolatedRoads();
	clipBoard->clear();

	for (int i = 0; i < history.size(); i++) {
		delete history[i];
	}
	history.clear();
}

void RoadGraphEditor::clearInterpolatedRoads() {
	// clear the interpolated roads
	for (int i = 0; i < interpolatedRoads.size(); i++) {
		delete interpolatedRoads[i];
	}
	interpolatedRoads.clear();
}

void RoadGraphEditor::openRoad(QString filename) {
	clear();

	GraphUtil::loadRoads(roads, filename, 7);
}

void RoadGraphEditor::openToAddRoad(QString filename) {
	if (selectedRoads == NULL) selectedRoads = new RoadGraph();
	GraphUtil::loadRoads(selectedRoads, filename, 7);

	// update the bbox according to the loaded roads
	if (selectedArea != NULL) {
		delete selectedArea;
	}
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoads));

	// inflate the bbox a little so that all the vertices are completely within the bbox.
	/*
	((BBox*)selectedArea)->minPt.setX(((BBox*)selectedArea)->minPt.x() - selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->minPt.setY(((BBox*)selectedArea)->minPt.y() - selectedArea->dy() * 0.1f);
	((BBox*)selectedArea)->maxPt.setX(((BBox*)selectedArea)->maxPt.x() + selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->maxPt.setY(((BBox*)selectedArea)->maxPt.y() + selectedArea->dy() * 0.1f);
	*/

	mode = MODE_AREA_SELECTED;

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
	if (selectedArea == NULL) return;

	history.push_back(GraphUtil::copyRoads(roads));

	// extract the roads within the area, and put it into the clipboard.
	clipBoard->copy(roads, *selectedArea);

	GraphUtil::subtractRoads(roads, *selectedArea, true);

	// clear the selected roads
	if (selectedRoads != NULL) {
		delete selectedRoads;
		selectedRoads = NULL;
	}
}

void RoadGraphEditor::copy() {
	if (mode != MODE_AREA_SELECTED) return;
	if (selectedArea == NULL) return;

	// extract the roads within the area, and put it into the clipboard.
	clipBoard->copy(roads, *selectedArea);
}

void RoadGraphEditor::paste() {
	if (clipBoard->empty()) return;

	if (selectedRoads != NULL) {
		delete selectedRoads;
	}
	selectedRoads = clipBoard->paste();

	// update the bbox according to the paseted roads
	if (selectedArea != NULL) {
		delete selectedArea;
	}
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoads));

	// move the pasted roads to the center of the screen (to be updated!!!)
	GraphUtil::translate(selectedRoads, selectedArea->midPt() * -1.0f);
	delete selectedArea;
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoads));

	// inflate the bbox a little so that all the vertices are completely within the bbox.
	((BBox*)selectedArea)->minPt.setX(((BBox*)selectedArea)->minPt.x() - selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->minPt.setY(((BBox*)selectedArea)->minPt.y() - selectedArea->dy() * 0.1f);
	((BBox*)selectedArea)->maxPt.setX(((BBox*)selectedArea)->maxPt.x() + selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->maxPt.setY(((BBox*)selectedArea)->maxPt.y() + selectedArea->dy() * 0.1f);

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
	if (selectedArea != NULL) delete selectedArea;
	selectedArea = new BBox(GraphUtil::getAABoundingBox(roads));
}

void RoadGraphEditor::startArea(const QVector2D& pt) {
	if (selectedArea != NULL) delete selectedArea;
	selectedArea = new BBox();
	((BBox*)selectedArea)->addPoint(pt);

	mode = MODE_DEFINING_AREA;
}

void RoadGraphEditor::updateArea(const QVector2D& pt) {
	((BBox*)selectedArea)->maxPt.setX(pt.x());
	((BBox*)selectedArea)->minPt.setY(pt.y());
}

void RoadGraphEditor::finalizeArea() {
	if (((BBox*)selectedArea)->maxPt.x() < ((BBox*)selectedArea)->minPt.x()) {
		float x = ((BBox*)selectedArea)->maxPt.x();
		((BBox*)selectedArea)->maxPt.setX(((BBox*)selectedArea)->minPt.x());
		((BBox*)selectedArea)->minPt.setX(x);
	}
	if (((BBox*)selectedArea)->maxPt.y() < ((BBox*)selectedArea)->minPt.y()) {
		float y = ((BBox*)selectedArea)->maxPt.y();
		((BBox*)selectedArea)->maxPt.setY(((BBox*)selectedArea)->minPt.y());
		((BBox*)selectedArea)->minPt.setY(y);
	}

	// if the box is just a single point, cancel the selection.
	if (((BBox*)selectedArea)->maxPt.x() == ((BBox*)selectedArea)->minPt.x() && ((BBox*)selectedArea)->maxPt.y() == ((BBox*)selectedArea)->minPt.y()) {
		mode = MODE_DEFAULT;
	} else {
		history.push_back(GraphUtil::copyRoads(roads));

		if (selectedRoads != NULL) {
			delete selectedRoads;
			selectedRoads = NULL;
		}

		// copy the roads in the area to "selectedRoads"
		selectedRoads = GraphUtil::copyRoads(roads);
		GraphUtil::extractRoads(selectedRoads, *selectedArea, true);
		GraphUtil::clean(selectedRoads);

		// subtract the area from the roads
		GraphUtil::subtractRoads(roads, *selectedArea, true);

		mode = MODE_AREA_SELECTED;
	}
}

void RoadGraphEditor::resizeAreaBR(const QVector2D& pt) {
	((BBox*)selectedArea)->maxPt.setX(pt.x());
	((BBox*)selectedArea)->minPt.setY(pt.y());
}

void RoadGraphEditor::startDistortingArea() {
	mode = MODE_DISTORTING_AREA;

	QVector2D leftPt(((BBox*)selectedArea)->minPt.x(), ((BBox*)selectedArea)->midPt().y());
	QVector2D rightPt(((BBox*)selectedArea)->maxPt.x(), ((BBox*)selectedArea)->midPt().y());
	float arc_len = selectedArea->dy();

	delete selectedArea;
	selectedArea = new ArcArea(leftPt, rightPt, 10000.0f, arc_len);

	// save the original roads
	if (selectedRoadsOrig != NULL) delete selectedRoadsOrig;
	selectedRoadsOrig = GraphUtil::copyRoads(selectedRoads);

	GraphUtil::distort(selectedRoads, selectedRoadsOrig, (ArcArea*)selectedArea);
}

void RoadGraphEditor::distortArea(float dx, float dy) {
	if (mode != MODE_DISTORTING_AREA) return;
	if (selectedArea == NULL) return;

	((ArcArea*)selectedArea)->radius += dx * 50.0f;
	if (((ArcArea*)selectedArea)->radius < 1000.0f) {
		((ArcArea*)selectedArea)->radius = 1000.0f;
	}
	((ArcArea*)selectedArea)->arc_len += dy * 15.0f;

	GraphUtil::distort(selectedRoads, selectedRoadsOrig, (ArcArea*)selectedArea);
}

void RoadGraphEditor::finalizeDistortArea() {
	if (selectedRoadsOrig != NULL) {
		delete selectedRoadsOrig;
		selectedRoadsOrig = NULL;
	}

	mode = MODE_AREA_SELECTED;
}

void RoadGraphEditor::moveArea(float dx, float dy) {
	if (mode != MODE_AREA_SELECTED) return;

	selectedArea->translate(dx, dy);

	GraphUtil::translate(selectedRoads, QVector2D(dx, dy));
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

/**
 * Interpolate the selected roads and the actual roads.
 */
void RoadGraphEditor::interpolate() {
	if (selectedRoads != NULL) {
		BBox bbox1 = GraphUtil::getAABoundingBox(roads);
		BBox bbox2 = GraphUtil::getAABoundingBox(selectedRoads);

		// if there is no intersection between two road graphs, connect two graphs instead.
		if (!bbox1.overlapsWithBBoxXY(bbox2)) {
			connectRoads();
			return;
		}

		RoadGraph* roads1 = GraphUtil::copyRoads(roads);
		GraphUtil::extractRoads(roads1, *selectedArea, true);
		GraphUtil::clean(roads1);

		// if there is no edge in the actual roads within the area, connect two graphs instead.
		if (GraphUtil::getNumEdges(roads1) == 0) {
			connectRoads();
			return;
		}

		// define the roots
		RoadVertexDesc root1 = GraphUtil::getVertex(roads1, bbox2.midPt());
		RoadVertexDesc root2 = GraphUtil::getVertex(selectedRoads, bbox2.midPt());

		// build trees
		BFSTree tree1(roads1, root1);
		BFSTree tree2(selectedRoads, root2);

		// find matching
		QMap<RoadVertexDesc, RoadVertexDesc> map1;
		QMap<RoadVertexDesc, RoadVertexDesc> map2;
		GraphUtil::findCorrespondence(roads1, &tree1, selectedRoads, &tree2, true, 1.5f, map1, map2);

		// clear the interpolated roads
		clearInterpolatedRoads();

		for (int i = 0; i <= 20; i++) {
			float ratio = (float)i / 20.0f;

			// interpolate
			RoadGraph* temp = GraphUtil::interpolate(roads1, selectedRoads, map1, 1 - ratio);
			interpolatedRoads.push_back(temp);

			// clear the "fullyPaired" flag of "roads1"
			RoadEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
				roads1->graph[*ei]->fullyPaired = false;
			}

			// clear the "fullyPaired" flag of "selectedRoads"
			for (boost::tie(ei, eend) = boost::edges(selectedRoads->graph); ei != eend; ++ei) {
				selectedRoads->graph[*ei]->fullyPaired = false;
			}
		}

		// delete temporary roads
		delete roads1;

		// clear the selected roads
		delete selectedRoads;

		selectedRoads = interpolatedRoads[0];

		// subtract the area from the roads
		GraphUtil::subtractRoads(roads, *selectedArea, true);
	}

	mode = MODE_AREA_SELECTED;
}

void RoadGraphEditor::showInterpolatedRoads(int ratio) {
	if (ratio < 0 || ratio >= interpolatedRoads.size()) return;

	selectedRoads = interpolatedRoads[ratio];
	selectedRoads->setModified();
}

void RoadGraphEditor::finalizeInterpolation(int ratio) {
	if (selectedRoads != NULL) {
		GraphUtil::mergeRoads(roads, selectedRoads);
	}
	clearInterpolatedRoads();

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
