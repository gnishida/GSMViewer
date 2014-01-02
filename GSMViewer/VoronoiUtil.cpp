#include "VoronoiUtil.h"
#include "GraphUtil.h"
#include "Util.h"

/**
 * Build Voronoi diagram.
 */
void VoronoiUtil::buildVoronoi(RoadGraph* roads1, RoadGraph* roads2, boost::polygon::voronoi_diagram<double>& vd, std::vector<VoronoiVertex>& points) {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		if (!roads1->graph[*vi]->valid) continue;

		points.push_back(VoronoiVertex(roads1, *vi));
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		if (!roads2->graph[*vi]->valid) continue;

		points.push_back(VoronoiVertex(roads2, *vi));
	}

	// Construction of the Voronoi Diagram.
	construct_voronoi(points.begin(), points.end(), &vd);
}

/**
 * Based on the existing road graph and the selected road graph, build a voronoi diagram.
 */
void VoronoiUtil::voronoi(RoadGraph* roads1, RoadGraph* roads2, VoronoiDiagram& voronoiDiagram) {
	voronoiDiagram.clear();

	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads1, *vi));
		voronoiDiagram.points.push_back(roads1->graph[*vi]->pt);
		voronoiDiagram.groups.push_back(0);
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads2, *vi));
		voronoiDiagram.points.push_back(roads2->graph[*vi]->pt);
		voronoiDiagram.groups.push_back(1);
		conv[cell_index] = *vi;
	}

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// create a voronoi edges
	for (boost::polygon::voronoi_diagram<double>::const_edge_iterator it = vd.edges().begin(); it != vd.edges().end(); ++it) {
		if (!it->is_primary()) continue;
		if (it->is_infinite()) continue;

		const boost::polygon::voronoi_diagram<double>::vertex_type* vertex0 = it->vertex0();
		const boost::polygon::voronoi_diagram<double>::vertex_type* vertex1 = it->vertex1();

		if (vertex0 != NULL && vertex1 != NULL) {
			RoadVertexPtr v0 = RoadVertexPtr(new RoadVertex(QVector2D((float)vertex0->x() * 0.01f, (float)vertex0->y() * 0.01f)));
			RoadVertexDesc v0_desc = boost::add_vertex(voronoiDiagram.edges.graph);
			voronoiDiagram.edges.graph[v0_desc] = v0;

			RoadVertexPtr v1 = RoadVertexPtr(new RoadVertex(QVector2D((float)vertex0->x() * 0.01f, (float)vertex0->y() * 0.01f)));
			RoadVertexDesc v1_desc = boost::add_vertex(voronoiDiagram.edges.graph);
			voronoiDiagram.edges.graph[v1_desc] = v1;

			GraphUtil::addEdge(&voronoiDiagram.edges, v0_desc, v1_desc, 1, 1, false);
		}
	}
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 各エッジについて、２つの頂点がそれぞれ属するセルが隣接していない場合、そのエッジを無効にする。
 * かなり、保守的なアルゴリズムだ。しかも、エッジを無効にするだけで、２つの道路網をつなぐエッジを追加していないので、
 * あまり良くない。
 */
void VoronoiUtil::merge(RoadGraph* roads1, RoadGraph* roads2) {
	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads1, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads2, *vi));
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

	roads1->setModified();
	roads2->setModified();
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 各エッジについて、２つの頂点がそれぞれ属するセルが隣接していない場合、
 * 1) 敵陣に近い場合、両端の頂点が共に敵陣の中なら、そのエッジを無効にする。
 * 2) 片方の頂点だけ敵陣の中なら、その頂点を敵陣の近接頂点にスナップする。
 */
void VoronoiUtil::merge2(RoadGraph* roads1, RoadGraph* roads2) {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads1) == 0) return;
	if (GraphUtil::getNumVertices(roads2) == 0) return;
	
	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	// define the center of the roads
	QVector2D center1 = roads1->graph[GraphUtil::getCentralVertex(roads1)]->pt;
	QVector2D center2 = roads2->graph[GraphUtil::getCentralVertex(roads2)]->pt;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads1, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads2, *vi));
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

		bool withinTerritory = isWithinTerritory(roads1, center1, roads2, center2, v);

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
				if (!isWithinTerritory(roads1, center1, roads2, center2, v) &&
					!isWithinTerritory(roads1, center1, roads2, center2, VoronoiVertex(v.roads, tgt))) {
					v.roads->graph[*ei]->valid = false;
					v.roads->graph[v.desc]->valid = false;
					continue;
				}

				// if only v.desc is outside the teritory, find the closest vertex from neighbors[i], and snap v.desc to it.
				if (v.roads == roads1) {
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads2, v.roads->graph[tgt]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, roads2->graph[snap_v_desc]->pt);
				} else {
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads1, v.roads->graph[tgt]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, roads1->graph[snap_v_desc]->pt);
				}
			}
		}
	}

	roads1->setModified();
	roads2->setModified();
}

/**
 * roads2のエリアが"area"で与えられている時の、merge
 */
void VoronoiUtil::merge2(RoadGraph* roads1, RoadGraph* roads2, AbstractArea* area) {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads1) == 0) return;
	if (GraphUtil::getNumVertices(roads2) == 0) return;
	
	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads1, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi, ++cell_index) {
		points.push_back(VoronoiVertex(roads2, *vi));
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

		bool withinTerritory = isWithinTerritory(roads1, roads2, *area, v);

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
				if (!isWithinTerritory(roads1, roads2, *area, VoronoiVertex(v.roads, tgt))) {
					v.roads->graph[*ei]->valid = false;
					v.roads->graph[v.desc]->valid = false;
					continue;
				}

				// if only v.desc is outside the teritory, find the closest vertex from neighbors[i], and snap v.desc to it.
				QVector2D vec = v.roads->graph[v.desc]->pt - v.roads->graph[tgt]->pt;
				if (v.roads == roads1) {
					//RoadVertexDesc snap_v_desc = GraphUtil::getVertex(selectedRoads, v.roads->graph[tgt]->pt, atan2f(vec.y(), vec.x()), 0.75f);
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads2, v.roads->graph[v.desc]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, roads2->graph[snap_v_desc]->pt);
				} else {
					//RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads, v.roads->graph[tgt]->pt, atan2f(vec.y(), vec.x()), 0.75f);
					RoadVertexDesc snap_v_desc = GraphUtil::getVertex(roads1, v.roads->graph[v.desc]->pt);
					GraphUtil::moveVertex(v.roads, v.desc, roads1->graph[snap_v_desc]->pt);
				}
			}
		}
	}

	roads1->setModified();
	roads2->setModified();
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 1) 各セルについて、１つ以上の隣接頂点uとVoronoi図で隣接していない場合、敵陣の中の場合、そのセルの頂点vを無効にする。
 * 2) 各セルについて、１つ以上の隣接頂点uとVoronoi図で隣接していない場合、
 *    自陣の中の場合、隣接する敵セルの頂点wについて、ベクトルu-vとベクトルw-vのなす角度が小さい場合、wを無効にする。
 * 3) 次に、各エッジについて、両端頂点が共に無効の場合、そのエッジも無効にする。
 * 4) 各エッジについて、両端頂点のどちらかが無効になっている場合、無効な頂点uに最も近い敵陣の頂点u'を探す。
 *    有効な頂点vとu'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
 *    1.x倍以下なら、エッジをu'にスナップする。ただし、そのエッジが他のエッジと交差する場合、無効にする。
 * 5) ２つの道路網間で、非常に近い頂点同士をくっつける。
 */
void VoronoiUtil::merge3(RoadGraph* roads1, RoadGraph* roads2) {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads1) == 0) return;
	if (GraphUtil::getNumVertices(roads2) == 0) return;

	// define the center of the roads
	QVector2D center1 = roads1->graph[GraphUtil::getCentralVertex(roads1)]->pt;
	QVector2D center2 = roads2->graph[GraphUtil::getCentralVertex(roads2)]->pt;

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	std::vector<VoronoiVertex> points;
	buildVoronoi(roads1, roads2, vd, points);

	// for each cell, check the adjacent cells
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		QList<RoadVertexDesc> neighbors;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
			if (!v.roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
			if (!v.roads->graph[tgt]->valid) continue;

			neighbors.push_back(tgt);
		}

		// check if the neighbors are adjacent in Voronoi diagram
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

		// 全ての隣接頂点とVoronoi図で隣接している場合は、スキップ
		if (neighbors.size() == 0) continue;

		// 1) 頂点vが敵陣の中の場合、そのセルの頂点を無効にする
		if (!isWithinTerritory(roads1, center1, roads2, center2, v)) {
			v.roads->graph[v.desc]->valid = false;
		}
	}

	// for each cell, check the adjacent cells
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		QList<RoadVertexDesc> neighbors;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
			if (!v.roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
			if (!v.roads->graph[tgt]->valid) continue;

			neighbors.push_back(tgt);
		}

		// check if the neighbors are adjacent in Voronoi diagram
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

		// 全ての隣接頂点とVoronoi図で隣接している場合は、スキップ
		if (neighbors.size() == 0) continue;

		if (!isWithinTerritory(roads1, center1, roads2, center2, v)) continue;

		// 2) 頂点vが自陣の中の場合、
		for (int i = 0; i < neighbors.size(); i++) {
			QVector2D vec = v.roads->graph[neighbors[i]]->pt - v.roads->graph[v.desc]->pt;

			invalidateObstacleEdges(cell, vec, points, 0.85f);
		}
	}

	// 3) 次に、各エッジについて、両端頂点が共にが無効の場合、そのエッジも無効にする。
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

		if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) {
			roads1->graph[*ei]->valid = false;
		}
	}
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		if (!roads2->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

		if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) {
			roads2->graph[*ei]->valid = false;
		}
	}

	// 4) 各エッジについて、両端頂点のどちらかが無効になっている場合、無効な頂点uに最も近い敵陣の頂点u'を探す。
	if (GraphUtil::getNumVertices(roads2) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			if (!roads1->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads1->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads1->graph[src]->valid && roads1->graph[tgt]->valid) continue;

			float len = roads1->graph[*ei]->getLength();
			RoadVertexDesc invalidVertexDesc;
			if (!roads1->graph[src]->valid) {
				invalidVertexDesc = src;
			} else {
				invalidVertexDesc = tgt;
			}

			RoadVertexDesc new_v_desc = GraphUtil::getVertex(roads2, roads1->graph[invalidVertexDesc]->pt);
			GraphUtil::moveVertex(roads1, invalidVertexDesc, roads2->graph[new_v_desc]->pt);
			roads1->graph[invalidVertexDesc]->valid = true;

			// 有効な頂点vとu'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads1->graph[*ei]->getLength() > len * 1.2f) {
				roads1->graph[*ei]->valid = false;
				continue;
			}

			// 既存エッジと交差するかチェック
			RoadOutEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::out_edges(invalidVertexDesc, roads1->graph); ei2 != eend2; ++ei2) {
				bool overlapped = false;
				if (GraphUtil::isIntersect(roads2, roads1->graph[*ei2]->polyLine)) {
					roads1->graph[*ei2]->valid = false;
				}
			}
		}
	}
	if (GraphUtil::getNumVertices(roads1) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
			if (!roads2->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads2->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads2->graph[src]->valid && roads2->graph[tgt]->valid) continue;

			float len = roads2->graph[*ei]->getLength();
			RoadVertexDesc invalidVertexDesc;
			if (!roads2->graph[src]->valid) {
				invalidVertexDesc = src;
			} else {
				invalidVertexDesc = tgt;
			}

			RoadVertexDesc new_v_desc = GraphUtil::getVertex(roads1, roads2->graph[invalidVertexDesc]->pt);
			GraphUtil::moveVertex(roads2, invalidVertexDesc, roads1->graph[new_v_desc]->pt);
			roads2->graph[invalidVertexDesc]->valid = true;

			// 有効な頂点vとu'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads2->graph[*ei]->getLength() > len * 1.2f) {
				roads2->graph[*ei]->valid = false;
				continue;
			}

			// 既存エッジと交差するかチェック
			RoadOutEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::out_edges(invalidVertexDesc, roads2->graph); ei2 != eend2; ++ei2) {
				bool overlapped = false;
				if (GraphUtil::isIntersect(roads1, roads2->graph[*ei2]->polyLine)) {
					roads2->graph[*ei2]->valid = false;
				}
			}
		}
	}

	GraphUtil::realize(roads1);
	GraphUtil::realize(roads2);

	// 5) ２つの道路網間で、非常に近い頂点同士をくっつける。
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		RoadVertexDesc near_desc;
		if (GraphUtil::getVertex(roads2, roads1->graph[*vi]->pt, 5.0f, near_desc)) {
			GraphUtil::moveVertex(roads1, *vi, roads2->graph[near_desc]->pt);
		}
	}
	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		RoadVertexDesc near_desc;
		if (GraphUtil::getVertex(roads1, roads2->graph[*vi]->pt, 5.0f, near_desc)) {
			GraphUtil::moveVertex(roads2, *vi, roads1->graph[near_desc]->pt);
		}
	}
}

/**
 * Voronoi図を使って、２つの道路網をうまく結合させる。
 * 1) 各セルについて、１つ以上の隣接頂点uとVoronoi図で隣接していない場合、敵陣の中の場合、そのセルの頂点vを無効にする。
 * 2) 各セルについて、１つ以上の隣接頂点uとVoronoi図で隣接していない場合、
 *    自陣の中の場合、隣接する敵セルの頂点wについて、ベクトルu-vとベクトルw-vのなす角度が小さい場合、wを無効にする。
 * 3) 次に、各エッジについて、両端頂点が共に無効の場合、そのエッジも無効にする。
 * 4) 各エッジについて、両端頂点のどちらかが無効になっている場合、有効な頂点vに最も近い敵陣の頂点v'を探す。
 *    有効な頂点vとv'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
 *    1.x倍以下なら、エッジをv'にスナップする。
 */
void VoronoiUtil::merge4(RoadGraph* roads1, RoadGraph* roads2) {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads1) == 0) return;
	if (GraphUtil::getNumVertices(roads2) == 0) return;

	std::vector<VoronoiVertex> points;
	QMap<int, RoadVertexDesc> conv;

	// define the center of the roads
	RoadVertexDesc root1 = GraphUtil::getCentralVertex(roads1);
	RoadVertexDesc root2 = GraphUtil::getCentralVertex(roads2);
	QVector2D center1 = roads1->graph[root1]->pt;
	QVector2D center2 = roads2->graph[root2]->pt;

	int cell_index = 0;
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi, ++cell_index) {
		if (!roads1->graph[*vi]->valid) continue;
		points.push_back(VoronoiVertex(roads1, *vi));
		conv[cell_index] = *vi;
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi, ++cell_index) {
		if (!roads2->graph[*vi]->valid) continue;
		points.push_back(VoronoiVertex(roads2, *vi));
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

		QList<RoadVertexDesc> neighbors;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
			if (!v.roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
			if (!v.roads->graph[tgt]->valid) continue;

			neighbors.push_back(tgt);
		}

		// check if the neighbors are adjacent in Voronoi diagram
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

		// 全ての隣接頂点とVoronoi図で隣接している場合は、スキップ
		if (neighbors.size() == 0) continue;

		// 1) 頂点vが敵陣の中の場合、そのセルの頂点を無効にする
		if (!isWithinTerritory(roads1, center1, roads2, center2, v)) {
			v.roads->graph[v.desc]->valid = false;
		}
	}

	// for each cell, check the adjacent cells
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		QList<RoadVertexDesc> neighbors;
		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v.desc, v.roads->graph); ei != eend; ++ei) {
			if (!v.roads->graph[*ei]->valid) continue;

			RoadVertexDesc tgt = boost::target(*ei, v.roads->graph);
			if (!v.roads->graph[tgt]->valid) continue;

			neighbors.push_back(tgt);
		}

		// check if the neighbors are adjacent in Voronoi diagram
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

		// 全ての隣接頂点とVoronoi図で隣接している場合は、スキップ
		if (neighbors.size() == 0) continue;

		if (!isWithinTerritory(roads1, center1, roads2, center2, v)) continue;

		// 2) 頂点vが自陣の中の場合、
		for (int i = 0; i < neighbors.size(); i++) {
			QVector2D vec1 = v.roads->graph[neighbors[i]]->pt - v.roads->graph[v.desc]->pt;
			
			edge = cell.incident_edge();

			do {
				if (!edge->is_primary()) continue;
		
				const boost::polygon::voronoi_diagram<double>::edge_type* neighbor_edge = edge->twin();
				const boost::polygon::voronoi_diagram<double>::cell_type* neighbor_cell = neighbor_edge->cell();
				int neighbor_index = neighbor_cell->source_index();

				QVector2D vec2 = points[neighbor_index].roads->graph[points[neighbor_index].desc]->pt - v.roads->graph[v.desc]->pt;

				if (GraphUtil::diffAngle(vec1, vec2) < 0.65f) {
					points[neighbor_index].roads->graph[points[neighbor_index].desc]->valid = false;
				}

				edge = edge->next();
			} while (edge != cell.incident_edge());
		}
	}

	// 3) 次に、各エッジについて、両端頂点が共に無効の場合、そのエッジも無効にする。
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

		if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) {
			roads1->graph[*ei]->valid = false;
		}
	}
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		if (!roads2->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

		if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) {
			roads2->graph[*ei]->valid = false;
		}
	}

	// 4) 各エッジについて、両端頂点のどちらかが無効になっている場合、有効な頂点vに最も近い敵陣の頂点v'を探す。
	if (GraphUtil::getNumVertices(roads2) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			if (!roads1->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads1->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads1->graph[src]->valid && roads1->graph[tgt]->valid) continue;

			float len = roads1->graph[*ei]->getLength();
			if (!roads1->graph[tgt]->valid) {
				QVector2D vec = roads1->graph[tgt]->pt - roads1->graph[src]->pt;
				float angle = atan2f(vec.y(), vec.x());
				RoadVertexDesc tgt2 = GraphUtil::getVertex(roads2, roads1->graph[src]->pt, angle, 1.2f);
				GraphUtil::moveVertex(roads1, tgt, roads2->graph[tgt2]->pt);
				roads1->graph[tgt]->valid = true;
			} else if (!roads1->graph[src]->valid) {
				QVector2D vec = roads1->graph[src]->pt - roads1->graph[tgt]->pt;
				float angle = atan2f(vec.y(), vec.x());
				RoadVertexDesc src2 = GraphUtil::getVertex(roads2, roads1->graph[tgt]->pt, angle, 1.2f);
				GraphUtil::moveVertex(roads1, src, roads2->graph[src2]->pt);
				roads1->graph[src]->valid = true;
			}

			// 有効な頂点vとv'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads1->graph[*ei]->getLength() > len * 1.2f) {
				//roads1->graph[*ei]->valid = false;
				continue;
			}
		}
	}
	if (GraphUtil::getNumVertices(roads1) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
			if (!roads2->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads2->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads2->graph[src]->valid && roads2->graph[tgt]->valid) continue;

			float len = roads2->graph[*ei]->getLength();
			if (!roads2->graph[tgt]->valid) {
				QVector2D vec = roads2->graph[tgt]->pt - roads2->graph[src]->pt;
				float angle = atan2f(vec.y(), vec.x());
				RoadVertexDesc tgt1 = GraphUtil::getVertex(roads1, roads2->graph[src]->pt, angle, 1.2f);
				GraphUtil::moveVertex(roads2, tgt, roads1->graph[tgt1]->pt);
				roads2->graph[tgt]->valid = true;
			} else if (!roads2->graph[src]->valid) {
				QVector2D vec = roads2->graph[src]->pt - roads2->graph[tgt]->pt;
				float angle = atan2f(vec.y(), vec.x());
				RoadVertexDesc src1 = GraphUtil::getVertex(roads1, roads2->graph[tgt]->pt, angle, 1.2f);
				GraphUtil::moveVertex(roads2, src, roads1->graph[src1]->pt);
				roads2->graph[src]->valid = true;
			}

			// 有効な頂点vとv'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads2->graph[*ei]->getLength() > len * 1.2f) {
				//roads2->graph[*ei]->valid = false;
				continue;
			}
		}
	}

	GraphUtil::realize(roads1);
	GraphUtil::realize(roads2);
}

/**
 * ボロノイ図を使わないで、もっと簡単に。
 * 1) 頂点vについて、敵陣の中にあり、且つ、敵頂点が近くにある場合、頂点vを無効にする。
 * 2) 次に、各エッジについて、両端頂点が共にが無効の場合、そのエッジも無効にする。
 * 3) 各エッジについて、両端頂点のどちらかが無効になっている場合、無効な頂点uに最も近い敵陣の頂点u'を探す。
 * 4) ２つの道路網間で、非常に近い頂点同士をくっつける。
 */
void VoronoiUtil::merge5(RoadGraph* roads1, RoadGraph* roads2) {
	// check if there is at least one vertex
	if (GraphUtil::getNumVertices(roads1) == 0) return;
	if (GraphUtil::getNumVertices(roads2) == 0) return;

	// define the center of the roads
	QVector2D center1 = roads1->graph[GraphUtil::getCentralVertex(roads1)]->pt;
	QVector2D center2 = roads2->graph[GraphUtil::getCentralVertex(roads2)]->pt;

	// 1)
	invalidateObstacleEdges(roads1, center1, roads2, center2);

	// 2) 次に、各エッジについて、両端頂点が共にが無効の場合、そのエッジも無効にする。
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

		if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) {
			roads1->graph[*ei]->valid = false;
		}
	}
	for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
		if (!roads2->graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads2->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

		if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) {
			roads2->graph[*ei]->valid = false;
		}
	}

	// 3) 各エッジについて、両端頂点のどちらかが無効になっている場合、無効な頂点uに最も近い敵陣の頂点u'を探す。
	if (GraphUtil::getNumVertices(roads2) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			if (!roads1->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads1->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads1->graph[src]->valid && !roads1->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads1->graph[src]->valid && roads1->graph[tgt]->valid) continue;

			float len = roads1->graph[*ei]->getLength();
			RoadVertexDesc invalidVertexDesc;
			if (!roads1->graph[src]->valid) {
				invalidVertexDesc = src;
			} else {
				invalidVertexDesc = tgt;
			}

			RoadVertexDesc new_v_desc = GraphUtil::getVertex(roads2, roads1->graph[invalidVertexDesc]->pt);
			GraphUtil::moveVertex(roads1, invalidVertexDesc, roads2->graph[new_v_desc]->pt);
			roads1->graph[invalidVertexDesc]->valid = true;

			// 有効な頂点vとu'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads1->graph[*ei]->getLength() > len * 1.2f) {
				roads1->graph[*ei]->valid = false;
				continue;
			}

			// 既存エッジと交差するかチェック
			RoadOutEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::out_edges(invalidVertexDesc, roads1->graph); ei2 != eend2; ++ei2) {
				bool overlapped = false;
				if (GraphUtil::isIntersect(roads2, roads1->graph[*ei2]->polyLine)) {
					roads1->graph[*ei2]->valid = false;
				}
			}
		}
	}
	if (GraphUtil::getNumVertices(roads1) > 0) {
		for (boost::tie(ei, eend) = boost::edges(roads2->graph); ei != eend; ++ei) {
			if (!roads2->graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads2->graph);
			RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

			// 両端頂点が共に無効の場合は、スキップ
			if (!roads2->graph[src]->valid && !roads2->graph[tgt]->valid) continue;

			// 両端頂点が共に有効の場合も、スキップ
			if (roads2->graph[src]->valid && roads2->graph[tgt]->valid) continue;

			float len = roads2->graph[*ei]->getLength();
			RoadVertexDesc invalidVertexDesc;
			if (!roads2->graph[src]->valid) {
				invalidVertexDesc = src;
			} else {
				invalidVertexDesc = tgt;
			}

			RoadVertexDesc new_v_desc = GraphUtil::getVertex(roads1, roads2->graph[invalidVertexDesc]->pt);
			GraphUtil::moveVertex(roads2, invalidVertexDesc, roads1->graph[new_v_desc]->pt);
			roads2->graph[invalidVertexDesc]->valid = true;

			// 有効な頂点vとu'との距離が、元のエッジの長さの1.x倍以上なら、エッジを無効にする。
			if (roads2->graph[*ei]->getLength() > len * 1.2f) {
				roads2->graph[*ei]->valid = false;
				continue;
			}

			// 既存エッジと交差するかチェック
			RoadOutEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::out_edges(invalidVertexDesc, roads2->graph); ei2 != eend2; ++ei2) {
				bool overlapped = false;
				if (GraphUtil::isIntersect(roads1, roads2->graph[*ei2]->polyLine)) {
					roads2->graph[*ei2]->valid = false;
				}
			}
		}
	}

	GraphUtil::realize(roads1);
	GraphUtil::realize(roads2);

	// 4) ２つの道路網間で、非常に近い頂点同士をくっつける。
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		RoadVertexDesc near_desc;
		if (GraphUtil::getVertex(roads2, roads1->graph[*vi]->pt, 5.0f, near_desc)) {
			GraphUtil::moveVertex(roads1, *vi, roads2->graph[near_desc]->pt);
		}
	}
	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		RoadVertexDesc near_desc;
		if (GraphUtil::getVertex(roads1, roads2->graph[*vi]->pt, 5.0f, near_desc)) {
			GraphUtil::moveVertex(roads2, *vi, roads1->graph[near_desc]->pt);
		}
	}
}

/**
 * ボロノイ頂点vが、自陣の中にいるならtrueを返却する。
 */
bool VoronoiUtil::isWithinTerritory(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2, const VoronoiVertex& v) {
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

/**
 * ボロノイ頂点vが、自陣の中にいるならtrueを返却する。
 */
bool VoronoiUtil::isWithinTerritory(RoadGraph* roads1, RoadGraph* roads2, const AbstractArea& area2, const VoronoiVertex& v) {
	if (v.roads == roads1) {
		if (!area2.contains(v.roads->graph[v.desc]->pt)) return true;
		else return false;
	} else {
		if (area2.contains(v.roads->graph[v.desc]->pt)) return true;
		else return false;
	}
}

/**
 * あるセル cell から、dir方向にある敵セルを、無効にする。
 */
void VoronoiUtil::invalidateObstacleEdges(const boost::polygon::voronoi_diagram<double>::cell_type& cell, const QVector2D& dir, std::vector<VoronoiVertex>& points, float threshold) {
	const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	std::size_t cell_index = cell.source_index();
	VoronoiVertex v = points[cell_index];

	do {
		if (!edge->is_primary()) continue;
		
		const boost::polygon::voronoi_diagram<double>::edge_type* neighbor_edge = edge->twin();
		const boost::polygon::voronoi_diagram<double>::cell_type* neighbor_cell = neighbor_edge->cell();
		int neighbor_index = neighbor_cell->source_index();

		// 敵セルの場合、方向をチェック
		if (v.roads != points[neighbor_index].roads) {
			QVector2D vec2 = points[neighbor_index].roads->graph[points[neighbor_index].desc]->pt - v.roads->graph[v.desc]->pt;

			if (GraphUtil::diffAngle(dir, vec2) < threshold) {
				points[neighbor_index].roads->graph[points[neighbor_index].desc]->valid = false;
			}
		}

		edge = edge->next();
	} while (edge != cell.incident_edge());
}

/**
 * 各エッジについて、敵陣の中にあり、且つ、近くに敵頂点がある場合、無効にする。
 */
void VoronoiUtil::invalidateObstacleEdges(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2) {
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads1->graph); vi != vend; ++vi) {
		if (!roads1->graph[*vi]->valid) continue;

		if (isWithinTerritory(roads1, center1, roads2, center2, VoronoiVertex(roads1, *vi))) continue;

		bool invalid = false;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads1->graph); ei != eend && !invalid; ++ei) {
			RoadVertexDesc tgt = boost::target(*ei, roads1->graph);

			std::vector<QVector2D> polyLine = GraphUtil::finerEdge(roads1, *ei, 200.0f);

			RoadEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::edges(roads2->graph); ei2 != eend2 && !invalid; ++ei2) {
				if (!roads2->graph[*ei2]->valid) continue;

				std::vector<QVector2D> polyLine2 = GraphUtil::finerEdge(roads2, *ei2, 200.0f);

				for (int i = 0; i < polyLine.size() && !invalid; i++) {
					for (int j = 0; j < polyLine2.size(); j++) {
						if ((polyLine[i] - polyLine2[j]).lengthSquared() < 40000.0f) {
							invalid = true;
							break;
						}
					}
				}
			}
		}

		if (invalid) {
			roads1->graph[*vi]->valid = false;
		}
	}

	for (boost::tie(vi, vend) = boost::vertices(roads2->graph); vi != vend; ++vi) {
		if (!roads2->graph[*vi]->valid) continue;

		if (isWithinTerritory(roads1, center1, roads2, center2, VoronoiVertex(roads2, *vi))) continue;

		bool invalid = false;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(*vi, roads2->graph); ei != eend && !invalid; ++ei) {
			RoadVertexDesc tgt = boost::target(*ei, roads2->graph);

			std::vector<QVector2D> polyLine = GraphUtil::finerEdge(roads2, *ei, 200.0f);

			RoadEdgeIter ei2, eend2;
			for (boost::tie(ei2, eend2) = boost::edges(roads1->graph); ei2 != eend2 && !invalid; ++ei2) {
				if (!roads1->graph[*ei2]->valid) continue;

				std::vector<QVector2D> polyLine2 = GraphUtil::finerEdge(roads1, *ei2, 200.0f);

				for (int i = 0; i < polyLine.size() && !invalid; i++) {
					for (int j = 0; j < polyLine2.size(); j++) {
						if ((polyLine[i] - polyLine2[j]).lengthSquared() < 40000.0f) {
							invalid = true;
							break;
						}
					}
				}
			}
		}

		if (invalid) {
			roads2->graph[*vi]->valid = false;
		}
	}
}
