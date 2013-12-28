#include "RoadGraphEditor.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include "ArcArea.h"
#include <boost/polygon/voronoi.hpp>

RoadGraphEditor::RoadGraphEditor() {
	roads = new RoadGraph();
	roadsOrig = new RoadGraph();
	selectedRoads = new RoadGraph();
	selectedRoadsOrig = new RoadGraph();
	roadDB = new RoadGraphDatabase("osm/3x3_simplified/new-york.gsm");

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

	mode = MODE_BASIC;

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

	// backup the road graph
	GraphUtil::copyRoads(roads, roadsOrig);
	GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

	mode = MODE_BASIC_AREA_SELECTED;
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
	if (mode != MODE_BASIC_AREA_SELECTED) return;
	if (selectedArea == NULL) return;

	history.push_back(GraphUtil::copyRoads(roads));

	// extract the roads within the area, and put it into the clipboard.
	clipBoard.copy(roads, *selectedArea);

	GraphUtil::subtractRoads(roads, *selectedArea, true);

	// clear the selected roads
	selectedRoads->clear();
	selectedRoadsOrig->clear();
}

void RoadGraphEditor::copy() {
	if (mode != MODE_BASIC_AREA_SELECTED) return;
	if (selectedArea == NULL) return;

	// extract the roads within the area, and put it into the clipboard.
	clipBoard.copy(roads, *selectedArea);
}

void RoadGraphEditor::paste() {
	if (clipBoard.empty()) return;

	delete selectedRoadsOrig;
	selectedRoadsOrig = clipBoard.paste();

	// update the bbox according to the paseted roads
	if (selectedArea != NULL) {
		delete selectedArea;
	}
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoadsOrig));

	// move the pasted roads to the center of the screen (to be updated!!!)
	GraphUtil::translate(selectedRoadsOrig, selectedArea->midPt() * -1.0f);
	delete selectedArea;
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoadsOrig));

	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	// inflate the bbox a little so that all the vertices are completely within the bbox.
	((BBox*)selectedArea)->minPt.setX(((BBox*)selectedArea)->minPt.x() - selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->minPt.setY(((BBox*)selectedArea)->minPt.y() - selectedArea->dy() * 0.1f);
	((BBox*)selectedArea)->maxPt.setX(((BBox*)selectedArea)->maxPt.x() + selectedArea->dx() * 0.1f);
	((BBox*)selectedArea)->maxPt.setY(((BBox*)selectedArea)->maxPt.y() + selectedArea->dy() * 0.1f);

	mode = MODE_BASIC_AREA_SELECTED;
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

	GraphUtil::copyRoads(roads, selectedRoads);
	roads->clear();

	// backup the road graph
	GraphUtil::copyRoads(roads, roadsOrig);
	GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

	mode = MODE_BASIC_AREA_SELECTED;
}

/**
 * 選択エリアを選択開始
 */
void RoadGraphEditor::startArea(const QVector2D& pt) {
	if (selectedArea != NULL) delete selectedArea;
	selectedArea = new BBox();
	((BBox*)selectedArea)->addPoint(pt);

	mode = MODE_BASIC_DEFINING_AREA;
}

/**
 * 選択エリアを選択中
 */
void RoadGraphEditor::updateArea(const QVector2D& pt) {
	((BBox*)selectedArea)->maxPt.setX(pt.x());
	((BBox*)selectedArea)->minPt.setY(pt.y());
}

/**
 * 選択エリアが確定した瞬間
 */
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
		mode = MODE_BASIC;
	} else {
		history.push_back(GraphUtil::copyRoads(roads));

		// 選択エリア内の道路セグメントを、"selectedRoads"にコピーする
		GraphUtil::copyRoads(roads, selectedRoads);
		GraphUtil::extractRoads(selectedRoads, *selectedArea, true);
		GraphUtil::clean(selectedRoads);

		// "roads"からは、選択エリアの分を削除する
		GraphUtil::subtractRoads(roads, *selectedArea, true);

		// backup the road graph
		GraphUtil::copyRoads(roads, roadsOrig);
		GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

		mode = MODE_BASIC_AREA_SELECTED;
	}
}

void RoadGraphEditor::startDistortingArea(int type) {
	mode = type;

	QVector2D leftPt(((BBox*)selectedArea)->minPt.x(), ((BBox*)selectedArea)->midPt().y());
	QVector2D rightPt(((BBox*)selectedArea)->maxPt.x(), ((BBox*)selectedArea)->midPt().y());
	float arc_len = selectedArea->dy();
	delete selectedArea;

	switch (mode) {
	case MODE_BASIC_AREA_DISTORTING_TL:
		selectedArea = new ArcArea(leftPt, rightPt, 10000.0f, arc_len);
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_LEFT;
		break;
	case MODE_BASIC_AREA_DISTORTING_TR:
		selectedArea = new ArcArea(leftPt, rightPt, -10000.0f, arc_len);
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_RIGHT;
		break;
	case MODE_BASIC_AREA_DISTORTING_BL:
		selectedArea = new ArcArea(leftPt, rightPt, 10000.0f, arc_len);
		selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_LEFT;
		break;
	case MODE_BASIC_AREA_DISTORTING_BR:
		selectedArea = new ArcArea(leftPt, rightPt, -10000.0f, arc_len);
		selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_RIGHT;
		break;
	}

	// copy the roads from the original
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);
	
	// distort the roads
	GraphUtil::distort(selectedRoads, (ArcArea*)selectedArea);
}

void RoadGraphEditor::distortArea(const QVector2D& pt) {
	selectedArea->resize(pt);

	// copy the roads from the original
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	// distort the roads
	GraphUtil::distort(selectedRoads, (ArcArea*)selectedArea);
}

void RoadGraphEditor::stopDistortingArea() {
	GraphUtil::copyRoads(selectedRoads, selectedRoadsOrig);

	mode = MODE_BASIC_AREA_SELECTED;
}

/**
 * 選択エリアを移動開始
 */
void RoadGraphEditor::startMovingArea() {
	history.push_back(GraphUtil::copyRoads(roads));

	mode = MODE_BASIC_AREA_MOVING;
}

/**
 * 選択エリアを移動中
 * 選択エリアは、既存道路網のInterpolationする
 */
void RoadGraphEditor::moveArea(float dx, float dy) {
	selectedArea->translate(dx, dy);

	GraphUtil::translate(selectedRoadsOrig, QVector2D(dx, dy));

	// copy back from the original
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);
	GraphUtil::copyRoads(roadsOrig, roads);

	voronoiCut2();
}

/**
 * 選択エリアを移動終了
 */
void RoadGraphEditor::stopMovingArea() {
	mode = MODE_BASIC_AREA_SELECTED;
}

/**
 * 選択エリアをリサイズ開始
 */
void RoadGraphEditor::startResizingArea(int type) {
	mode = type;

	switch (mode) {
	case MODE_BASIC_AREA_DISTORTING_TL:
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_LEFT;
		break;
	case MODE_BASIC_AREA_DISTORTING_TR:
		selectedArea->resizingType = AbstractArea::RESIZING_TOP_RIGHT;
		break;
	case MODE_BASIC_AREA_DISTORTING_BL:
		selectedArea->resizingType = AbstractArea::RESIZING_BOTTOM_LEFT;
		break;
	case MODE_BASIC_AREA_DISTORTING_BR:
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
	mode = MODE_BASIC_AREA_SELECTED;
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
	mode = MODE_BASIC_VERTEX_SELECTED;

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

	mode = MODE_BASIC_EDGE_SELECTED;

	return true;
}

void RoadGraphEditor::startMovingVertex() {
	history.push_back(GraphUtil::copyRoads(roads));

	mode = MODE_BASIC_VERTEX_MOVING;
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
	mode = MODE_BASIC_VERTEX_SELECTED;
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

	mode = MODE_BASIC;
}

/**
 * Merge the selected roads to the actual roads. 
 * By tring to connect the selected roads to the other as much as possible, the resulting roads hopefully look natural.
 */
void RoadGraphEditor::connectRoads() {
	GraphUtil::connectRoads(roads, selectedRoads, 60.0f);
	selectedRoads->clear();
	selectedRoadsOrig->clear();

	mode = MODE_BASIC;
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

	mode = MODE_BASIC_AREA_SELECTED;
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

	mode = MODE_BASIC;
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

/**
 * Based on the existing road graph and the selected road graph, build a voronoi diagram.
 */
void RoadGraphEditor::voronoi() {
	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(selectedRoadsOrig->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(selectedRoadsOrig, *vi));
		conv[cell_index] = *vi;
	}

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// create a voronoi edges
	voronoiGraph.clear();
	for (boost::polygon::voronoi_diagram<double>::const_edge_iterator it = vd.edges().begin(); it != vd.edges().end(); ++it) {
		if (!it->is_primary()) continue;
		if (it->is_infinite()) continue;

		const boost::polygon::voronoi_diagram<double>::vertex_type* vertex0 = it->vertex0();
		const boost::polygon::voronoi_diagram<double>::vertex_type* vertex1 = it->vertex1();

		if (vertex0 != NULL && vertex1 != NULL) {
			RoadVertexPtr v0 = RoadVertexPtr(new RoadVertex(QVector2D((float)vertex0->x() * 0.01f, (float)vertex0->y() * 0.01f)));
			RoadVertexDesc v0_desc = boost::add_vertex(voronoiGraph.graph);
			voronoiGraph.graph[v0_desc] = v0;

			RoadVertexPtr v1 = RoadVertexPtr(new RoadVertex(QVector2D((float)vertex0->x() * 0.01f, (float)vertex0->y() * 0.01f)));
			RoadVertexDesc v1_desc = boost::add_vertex(voronoiGraph.graph);
			voronoiGraph.graph[v1_desc] = v1;

			GraphUtil::addEdge(&voronoiGraph, v0_desc, v1_desc, 1, 1, false);
		}
	}
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 各エッジについて、２つの頂点がそれぞれ属するセルが隣接していない場合、そのエッジを無効にする。
 * かなり、保守的なアルゴリズムだ。しかも、エッジを無効にするだけで、２つの道路網をつなぐエッジを追加していないので、
 * あまり良くない。
 */
void RoadGraphEditor::voronoiCut() {
	// copy the road graph
	GraphUtil::copyRoads(selectedRoadsOrig, selectedRoads);

	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(selectedRoads->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(selectedRoads, *vi));
		conv[cell_index] = *vi;
	}

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// for each cell, check the adjacent cells
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		// list up all the outing edges
		QList<RoadVertexDesc> neighbors;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
			if (!v.roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
			neighbors.push_back(tgt);
		}

		// check if the neighbors are in the same road graph
		do {
			if (!edge->is_primary()) continue;
		
			const boost::polygon::voronoi_diagram<double>::edge_type* neighbor_edge = edge->twin();
			const boost::polygon::voronoi_diagram<double>::cell_type* neighbor_cell = neighbor_edge->cell();
			int neighbor_index = neighbor_cell->source_index();
			if (v.roads == points[neighbor_index].roads) {
				neighbors.removeOne(points[neighbor_index].desc);
			}	

			edge = edge->next();
		} while (edge != cell.incident_edge());

		// for those neighbors which are in the different road graph, remove the corresponding edges
		for (int i = 0; i < neighbors.size(); i++) {
			RoadEdgeDesc e = GraphUtil::getEdge(v.roads, v.desc, neighbors[i]);
			v.roads->graph[e]->valid = false;
		}
	}

	roads->setModified();
	selectedRoads->setModified();
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 各エッジについて、２つの頂点がそれぞれ属するセルが隣接していない場合、
 * 1) 敵陣に近い場合、両端の頂点が共に敵陣の中なら、そのエッジを無効にする。
 * 2) 片方の頂点だけ敵陣の中なら、その頂点を敵陣の近接頂点にスナップする。
 */
void RoadGraphEditor::voronoiCut2() {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads) == 0) return;
	if (GraphUtil::getNumVertices(selectedRoads) == 0) return;
	
	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	// define the center of the roads
	QVector2D center1 = roads->graph[GraphUtil::getCentralVertex(roads)]->pt;
	QVector2D center2 = selectedRoads->graph[GraphUtil::getCentralVertex(selectedRoads)]->pt;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads, *vi));
		conv[cell_index] = *vi;
	}

	qDebug() << cell_index;

	for (boost::tie(vi, vend) = boost::vertices(selectedRoads->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(selectedRoads, *vi));
		conv[cell_index] = *vi;
	}

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// for each cell, check the adjacent cells
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		bool withinTerritory = isWithinTerritory(roads, center1, selectedRoads, center2, v);

		// check if the neighbors are in the same road graph
		bool adjacentToEnemy = false;
		do {
			if (!edge->is_primary()) continue;
		
			const boost::polygon::voronoi_diagram<double>::edge_type* neighbor_edge = edge->twin();
			const boost::polygon::voronoi_diagram<double>::cell_type* neighbor_cell = neighbor_edge->cell();
			int neighbor_index = neighbor_cell->source_index();
			if (v.roads != points[neighbor_index].roads) {
				adjacentToEnemy = true;
				break;
			}

			edge = edge->next();
		} while (edge != cell.incident_edge());

		// if this cell is far away from the center of the road and is adjacent to the enemy cells,
		// remove the edge if both vertices are outside the territory.
		if (!withinTerritory && adjacentToEnemy) {
			// for each outing edge
			RoadOutEdgeIter ei, eend;
			for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
				if (!v.roads->graph[*ei]->valid) continue;

				RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
				
				// if both the vertices is outside the teritory, remove this edge
				if (!isWithinTerritory(roads, center1, selectedRoads, center2, v) &&
					!isWithinTerritory(roads, center1, selectedRoads, center2, VoronoiVertex(v.roads, tgt))) {
					v.roads->graph[*ei]->valid = false;
					v.roads->graph[v.desc]->valid = false;
					continue;
				}

				// if only v.desc is outside the teritory, find the closest vertex from neighbors[i], and snap v.desc to it.
				if (v.roads == roads) {
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(selectedRoads, v.roads->graph[tgt]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, selectedRoads->graph[snap_v_desc]->pt);
				} else {
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads, v.roads->graph[tgt]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, roads->graph[snap_v_desc]->pt);
				}
			}
		}
	}

	roads->setModified();
	selectedRoads->setModified();
}

bool RoadGraphEditor::isWithinTerritory(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2, const VoronoiVertex& v) {
	if (v.roads == roads1) {
		float dist1 = (v.roads->graph[v.desc]->pt - center1).length();
		float dist2 = (v.roads->graph[v.desc]->pt - center2).length();
		if (dist1 <= dist2) return true;
		else return false;
	} else {
		float dist1 = (v.roads->graph[v.desc]->pt - center2).length();
		float dist2 = (v.roads->graph[v.desc]->pt - center1).length();
		if (dist1 <= dist2) return true;
		else return false;
	}
}

void RoadGraphEditor::startSketchLine(const QVector2D& pt, float snap_threshold) {
	sketch.startLine(pt, snap_threshold);

	mode = MODE_SKETCH_SKETCHING;
}

void RoadGraphEditor::finalizeSketchLine(float simplify_threshold, float snap_threshold) {
	sketch.finalizeLine(simplify_threshold, snap_threshold);

	roadDB->findSimilarRoads(&sketch, 1, shadowRoads);

	mode = MODE_SKETCH;
}

void RoadGraphEditor::instanciateShadowRoads() {
	GraphUtil::copyRoads(shadowRoads[0]->roads, selectedRoadsOrig);

	if (selectedArea != NULL) {
		delete selectedArea;
	}
	selectedArea = new BBox(GraphUtil::getAABoundingBox(selectedRoadsOrig));
	
	// clear the sketch
	sketch.clear();

	// clear the shadow roads
	for (int i = 0; i < shadowRoads.size(); i++) {
		delete shadowRoads[i];
	}
	shadowRoads.clear();

	mode = MODE_BASIC_AREA_SELECTED;
}
