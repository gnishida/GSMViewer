#pragma once

#include "RoadGraph.h"
#include "VoronoiDiagram.h"
#include "AbstractArea.h"
#include <boost/polygon/voronoi.hpp>

class VoronoiVertex {
public:
	RoadGraph* roads;
	RoadVertexDesc desc;

public:
	VoronoiVertex(RoadGraph* roads, RoadVertexDesc desc) {
		this->roads = roads;
		this->desc = desc;
	}
};

namespace boost {
namespace polygon {
template <>
struct geometry_concept<VoronoiVertex> { typedef point_concept type; };
  
template <>
struct point_traits<VoronoiVertex> {
	typedef int coordinate_type;
   
	static inline coordinate_type get(const VoronoiVertex& v, orientation_2d orient) {
		return (orient == HORIZONTAL) ? v.roads->graph[v.desc]->pt.x() * 100 : v.roads->graph[v.desc]->pt.y() * 100;
	}
};
}
}

class VoronoiUtil {
protected:
	VoronoiUtil() {}
	~VoronoiUtil() {}

public:
	static void voronoi(RoadGraph* roads1, RoadGraph* roads2, VoronoiDiagram& vd);
	static void merge(RoadGraph* roads1, RoadGraph* roads2);
	static void merge2(RoadGraph* roads1, RoadGraph* roads2);
	static void merge2(RoadGraph* roads1, RoadGraph* roads2, AbstractArea* area);
	static void merge3(RoadGraph* roads1, RoadGraph* roads2);
	static void merge4(RoadGraph* roads1, RoadGraph* roads2);

	static bool isWithinTerritory(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2, const VoronoiVertex& v);
	static bool isWithinTerritory(RoadGraph* roads1, RoadGraph* roads2, const AbstractArea& area2, const VoronoiVertex& v);
};

