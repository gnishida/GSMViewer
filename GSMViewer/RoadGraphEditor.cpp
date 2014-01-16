#include "RoadGraphEditor.h"
#include "GraphUtil.h"
#include "Util.h"
#include "BFSTree.h"
#include "ArcArea.h"
#include "CircleArea.h"
#include "VoronoiUtil.h"
#include <boost/polygon/voronoi.hpp>

RoadGraphEditor::RoadGraphEditor() {
	roads = new RoadGraph();
	roadsOrig = new RoadGraph();
	selectedRoads = new RoadGraph();
	selectedRoadsOrig = new RoadGraph();

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
	selectedArea = NULL;
	clearInterpolatedRoads();
	clipBoard.clear();

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
	GraphUtil::loadRoads(selectedRoads, filename, 7);

	// update the bbox according to the loaded roads
	selectedArea = BBoxPtr(new BBox(GraphUtil::getAABoundingBox(selectedRoads)));

	// backup the road graph
	GraphUtil::copyRoads(roads, roadsOrig);
	GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

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
	clipBoard.copy(roads, *selectedArea);

	GraphUtil::subtractRoads2(roads, *selectedArea);

	// clear the selected roads
	selectedRoads->clear();
	selectedRoadsOrig->clear();
}

void RoadGraphEditor::copy() {
	if (mode != MODE_AREA_SELECTED) return;
	if (selectedArea == NULL) return;

	// extract the roads within the area, and put it into the clipboard.
	clipBoard.copy(roads, *selectedArea);
}

void RoadGraphEditor::paste() {
	if (clipBoard.empty()) return;

	delete selectedRoadsOrig;
	selectedRoadsOrig = clipBoard.paste();

	// update the bbox according to the paseted roads
	selectedArea = BBoxPtr(new BBox(GraphUtil::getAABoundingBox(selectedRoadsOrig)));

	// move the pasted roads to the center of the screen (to be updated!!!)
	GraphUtil::translate(selectedRoadsOrig, selectedArea->midPt() * -1.0f);
	selectedArea = BBoxPtr(new BBox(GraphUtil::getAABoundingBox(selectedRoadsOrig)));

	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

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
	selectedArea = BBoxPtr(new BBox(GraphUtil::getAABoundingBox(roads)));

	GraphUtil::copyRoads(roads, selectedRoads);
	roads->clear();

	// backup the road graph
	GraphUtil::copyRoads(roads, roadsOrig);
	GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

	mode = MODE_AREA_SELECTED;
}

/**
 * 選択エリアを選択開始
 */
void RoadGraphEditor::startDefiningArea(const QVector2D& pt) {
	selectedArea = BBoxPtr(new BBox(pt));

	mode = MODE_DEFINING_AREA;
}

/**
 * 選択エリアを選択中
 */
void RoadGraphEditor::defineArea(const QVector2D& pt) {
	selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_RIGHT;
	selectedArea->resize(pt);
}

/**
 * 選択エリアが確定した瞬間
 */
void RoadGraphEditor::stopDefiningArea() {
	// if the box is just a single point, cancel the selection.
	if (selectedArea->dx() == 0.0f && selectedArea->dy() == 0.0f) {
		mode = MODE_DEFAULT;
	} else {
		history.push_back(GraphUtil::copyRoads(roads));

		// 選択エリア内の道路セグメントを、"selectedRoads"にコピーする
		GraphUtil::copyRoads(roads, selectedRoads);
		GraphUtil::extractRoads2(selectedRoads, *selectedArea);
		GraphUtil::clean(selectedRoads);

		// "roads"からは、選択エリアの分を削除する
		GraphUtil::subtractRoads2(roads, *selectedArea);
		GraphUtil::clean(roads);

		// backup the road graph
		GraphUtil::copyRoads(roads, roadsOrig);
		GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

		mode = MODE_AREA_SELECTED;
	}
}

void RoadGraphEditor::startDistortingArea(int type) {
	mode = type;

	selectedArea = ArcAreaPtr(new ArcArea(*selectedArea, mode));

	// copy back from the original
	GraphUtil::copyRoads(roadsOrig, roads);
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	// distort the roads
	GraphUtil::distort(selectedRoads, (const ArcArea&)*selectedArea);

	VoronoiUtil::merge2(roads, selectedRoads, *selectedArea);
}

void RoadGraphEditor::distortArea(const QVector2D& pt) {
	selectedArea->resize(pt);

	// copy back from the original
	GraphUtil::copyRoads(roadsOrig, roads);
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	// distort the roads
	GraphUtil::distort(selectedRoads, (const ArcArea&)*selectedArea);

	VoronoiUtil::merge2(roads, selectedRoads, *selectedArea);
}

void RoadGraphEditor::stopDistortingArea() {
	mode = MODE_AREA_SELECTED;
}

/**
 * 選択エリアを移動開始
 */
void RoadGraphEditor::startMovingArea() {
	history.push_back(GraphUtil::copyRoads(roads));

	mode = MODE_AREA_MOVING;
}

/**
 * 選択エリアを移動中
 * 選択エリアは、既存道路網のInterpolationする
 */
void RoadGraphEditor::moveArea(float dx, float dy) {
	selectedArea->translate(dx, dy);

	GraphUtil::translate(selectedRoadsOrig, QVector2D(dx, dy));

	// copy back from the original
	GraphUtil::copyRoads(roadsOrig, roads);
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	GraphUtil::clean(roads);
	GraphUtil::clean(selectedRoads);

	VoronoiUtil::merge5(roads, selectedRoads);
}

/**
 * 選択エリアを移動終了
 */
void RoadGraphEditor::stopMovingArea() {
	mode = MODE_AREA_SELECTED;
}

/**
 * 選択エリアをリサイズ開始
 */
void RoadGraphEditor::startResizingArea(int type) {
	mode = type;

	switch (mode) {
	case MODE_AREA_DISTORTING_TL:
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_LEFT;
		break;
	case MODE_AREA_DISTORTING_TR:
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_RIGHT;
		break;
	case MODE_AREA_DISTORTING_BL:
		selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_LEFT;
		break;
	case MODE_AREA_DISTORTING_BR:
		selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_RIGHT;
		break;
	}
}

/**
 * 選択エリアをリサイズ中
 */
void RoadGraphEditor::resizeArea(const QVector2D& pt) {
	selectedArea->resize(pt);
}

/**
 * 選択エリアをリサイズ終了
 */
void RoadGraphEditor::stopResizingArea() {
	mode = MODE_AREA_SELECTED;
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

void RoadGraphEditor::startMovingVertex() {
	history.push_back(GraphUtil::copyRoads(roads));

	mode = MODE_VERTEX_MOVING;
}

void RoadGraphEditor::moveVertex(const QVector2D& pt) {
	if (roads == NULL) return;
	
	GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
}

void RoadGraphEditor::moveVertex(const QVector2D& pt, float snap_threshold) {
	if (roads == NULL) return;

	// if there is a vertex close to this point, snap to it.
	RoadVertexDesc desc;
	if (GraphUtil::getVertex(roads, pt, snap_threshold, selectedVertexDesc, desc)) {
		GraphUtil::moveVertex(roads, selectedVertexDesc, roads->graph[desc]->pt);
	} else {
		GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
	}
}

void RoadGraphEditor::stopMovingVertex() {
	mode = MODE_VERTEX_SELECTED;
}

void RoadGraphEditor::stopMovingVertex(float snap_threshold) {
	// if there is a vertex close to this point, snap to it.
	RoadVertexDesc desc;
	if (GraphUtil::getVertex(roads, roads->graph[selectedVertexDesc]->pt, snap_threshold, selectedVertexDesc, desc)) {
		GraphUtil::snapVertex(roads, selectedVertexDesc, desc);
	}
}

/**
 * Merge the selected roads to the actual roads.
 */
void RoadGraphEditor::unselectRoads() {
	GraphUtil::mergeRoads(roads, selectedRoads);
	selectedRoads->clear();
	selectedRoadsOrig->clear();

	// clear the area
	selectedArea = AbstractAreaPtr();

	mode = MODE_DEFAULT;
}

/**
 * Merge the selected roads to the actual roads. 
 * By tring to connect the selected roads to the other as much as possible, the resulting roads hopefully look natural.
 */
void RoadGraphEditor::connectRoads() {
	GraphUtil::connectRoads(roads, selectedRoads, 60.0f);
	selectedRoads->clear();
	selectedRoadsOrig->clear();

	mode = MODE_DEFAULT;
}

/**
 * Interpolate the selected roads and the actual roads.
 */
void RoadGraphEditor::interpolate() {
	BBox bbox1 = GraphUtil::getAABoundingBox(roads);
	BBox bbox2 = GraphUtil::getAABoundingBox(selectedRoadsOrig);

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
	RoadVertexDesc root2 = GraphUtil::getVertex(selectedRoadsOrig, bbox2.midPt());

	// build trees
	BFSTree tree1(roads1, root1);
	BFSTree tree2(selectedRoadsOrig, root2);

	// find matching
	QMap<RoadVertexDesc, RoadVertexDesc> map1;
	QMap<RoadVertexDesc, RoadVertexDesc> map2;
	GraphUtil::findCorrespondence(roads1, &tree1, selectedRoadsOrig, &tree2, true, 1.5f, map1, map2);

	// clear the interpolated roads
	clearInterpolatedRoads();

	for (int i = 0; i <= 20; i++) {
		float ratio = (float)i / 20.0f;

		// interpolate
		RoadGraph* temp = GraphUtil::interpolate(roads1, selectedRoadsOrig, map1, 1 - ratio);
		interpolatedRoads.push_back(temp);

		// clear the "fullyPaired" flag of "roads1"
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			roads1->graph[*ei]->fullyPaired = false;
		}

		// clear the "fullyPaired" flag of "selectedRoads"
		for (boost::tie(ei, eend) = boost::edges(selectedRoadsOrig->graph); ei != eend; ++ei) {
			selectedRoadsOrig->graph[*ei]->fullyPaired = false;
		}
	}

	// delete temporary roads
	delete roads1;

	// clear the selected roads
	delete selectedRoads;

	selectedRoads = interpolatedRoads[0];

	// subtract the area from the roads
	GraphUtil::subtractRoads(roads, *selectedArea, true);

	mode = MODE_AREA_SELECTED;
}

void RoadGraphEditor::showInterpolatedRoads(int ratio) {
	if (ratio < 0 || ratio >= interpolatedRoads.size()) return;

	selectedRoads = interpolatedRoads[ratio];
	selectedRoads->setModified();
}

void RoadGraphEditor::finalizeInterpolation(int ratio) {
	GraphUtil::mergeRoads(roads, selectedRoads);
	clearInterpolatedRoads();

	selectedRoads = new RoadGraph();
	selectedRoadsOrig->clear();

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

void RoadGraphEditor::voronoi() {
	VoronoiUtil::voronoi(roads, selectedRoads, voronoiDiagram);
}

void RoadGraphEditor::voronoiMerge() {
	VoronoiUtil::merge(roads, selectedRoads);
}

void RoadGraphEditor::voronoiMerge2() {
	VoronoiUtil::merge2(roads, selectedRoads);
}

void RoadGraphEditor::voronoiMerge2(const AbstractArea& area) {
	VoronoiUtil::merge2(roads, selectedRoads, area);
}

void RoadGraphEditor::voronoiMerge3() {
	VoronoiUtil::merge3(roads, selectedRoads);
}

/**
 * 既存の道路網roadsと、選択された道路網selectedRoadsを、簡単に結合する。
 * オーバラップされたエッジについて、中央から遠いほうを削除する。
 * 削除されたエッジについて、交点までのエッジに置き換える。
 */
void RoadGraphEditor::simpleConnect() {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads) == 0) return;
	if (GraphUtil::getNumVertices(selectedRoads) == 0) return;

	float tab, tcd;
	QVector2D intPt;

	// define the center of each road graph
	RoadVertexDesc root1 = GraphUtil::getCentralVertex(roads);
	RoadVertexDesc root2 = GraphUtil::getCentralVertex(selectedRoads);
	QVector2D center1 = roads->graph[root1]->pt;
	QVector2D center2 = selectedRoads->graph[root2]->pt;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(selectedRoads->graph); ei != eend; ++ei) {
		if (!selectedRoads->graph[*ei]->valid) continue;

		bool overlapped = false;

		RoadEdgeIter ei2, eend2;
		for (boost::tie(ei2, eend2) = boost::edges(roads->graph); ei2 != eend2 && !overlapped; ++ei2) {
			if (!roads->graph[*ei2]->valid) continue;

			for (int i = 0; i < selectedRoads->graph[*ei]->polyLine.size() - 1 && !overlapped; i++) {
				for (int j = 0; j < roads->graph[*ei2]->polyLine.size() - 1; j++) {
					if (Util::segmentSegmentIntersectXY(selectedRoads->graph[*ei]->polyLine[i], selectedRoads->graph[*ei]->polyLine[i + 1], roads->graph[*ei2]->polyLine[j], roads->graph[*ei2]->polyLine[j + 1], &tab, &tcd, true, intPt)) {
						if ((intPt - center1).lengthSquared() < (intPt - center2).lengthSquared()) {
							overlapped = true;
						} else {
							roads->graph[*ei2]->valid = false;
						}
						break;
					}
				}
			}
		}

		if (overlapped) {
			selectedRoads->graph[*ei]->valid = false;
		}
	}
}
