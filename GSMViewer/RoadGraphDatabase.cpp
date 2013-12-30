#include "RoadGraphDatabase.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include "TopNSearch.h"

RoadGraphDatabase::RoadGraphDatabase() {
	roads = NULL;
	roadsForSearch = NULL;
}

RoadGraphDatabase::~RoadGraphDatabase() {
	delete roads;
	clearTrees();
}

void RoadGraphDatabase::load(int type, QString filename) {
	this->type = type;

	if (this->roads != NULL) delete this->roads;
	if (this->roadsForSearch != NULL) delete this->roadsForSearch;

	this->roads = new RoadGraph();
	GraphUtil::loadRoads(roads, filename);

	// set up the road for search according to the type
	if (type == TYPE_LARGE) {
		roadsForSearch = GraphUtil::copyRoads(roads, 6);
	} else {
		roadsForSearch = GraphUtil::copyRoads(roads, 3);
	}
	GraphUtil::reduce(roadsForSearch);
	GraphUtil::clean(roadsForSearch);

	// compute the BFSTree for all the vertices
	createTrees(roadsForSearch);
}

void RoadGraphDatabase::findSimilarRoads(RoadGraph* roads1, int N, QList<ShadowRoadGraph*>& results) {
	// clear the results
	for (int i = 0; i < results.size(); i++) {
		delete results[i];
	}
	results.clear();

	TopNSearch<RoadVertexDesc> search;

	// if there is no edge, stop the similarity computation.
	if (GraphUtil::getNumEdges(roads1) == 0) return;

	// Find the central vertex in the sketch
	RoadVertexDesc root1 = GraphUtil::getCentralVertex(roads1);

	// Create a tree
	BFSTree tree1(roads1, root1);

	float max_similarity = -1.0f;
	BFSTree* min_tree2;
	for (QMap<RoadVertexDesc, BFSTree*>::iterator it = trees.begin(); it != trees.end(); ++it) {
		BFSTree* tree2 = it.value();

		// Find the matching
		QMap<RoadVertexDesc, RoadVertexDesc> map1;
		QMap<RoadVertexDesc, RoadVertexDesc> map2;
		GraphUtil::findCorrespondence(roads1, &tree1, roadsForSearch, tree2, false, 0.75f, map1, map2);

		// Compute the similarity
		float similarity = GraphUtil::computeSimilarity(roads1, map1, roadsForSearch, map2, 1.0f, 1.0f, 1.0f);
		search.add(similarity, it.key());

		if (similarity > max_similarity) {
			min_tree2 = tree2;
			max_similarity = similarity;
		}

		// clear the "fullyPaired" flags
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			roads1->graph[*ei]->fullyPaired = false;
		}
		for (boost::tie(ei, eend) = boost::edges(roadsForSearch->graph); ei != eend; ++ei) {
			roadsForSearch->graph[*ei]->fullyPaired = false;
		}
	}

	// retreive the top N
	QList<RoadVertexDesc> list = search.topN(N, TopNSearch<RoadVertexDesc>::ORDER_DESC);
	for (int i = 0; i < list.size(); i++) {
		RoadVertexDesc root2 = list[i];
		BFSTree* tree2 = trees[root2];

		RoadGraph* r2 = GraphUtil::copyRoads(roadsForSearch);

		// Find the matching
		QMap<RoadVertexDesc, RoadVertexDesc> map1;
		QMap<RoadVertexDesc, RoadVertexDesc> map2;
		GraphUtil::findCorrespondence(roads1, &tree1, r2, tree2, false, 0.75f, map1, map2);

		float similarity = GraphUtil::computeSimilarity(roads1, map1, r2, map2, 1.0f, 1.0f, 1.0f);
		
		// define the offset
		QVector2D translation = roads1->graph[root1]->pt - r2->graph[root2]->pt;

		// define the angle
		float rotation = computeRotationAngle(roads1, r2, map1);
		//float rotation = computeRotationAngle(r2, roads1, map1);

		//results.push_back(new ShadowRoadGraph(r2, roads, type, roads1->graph[root1]->pt, rotation, translation));
		results.push_back(new ShadowRoadGraph(r2, roads, type, r2->graph[root2]->pt, rotation, translation));

		// clear the "fullyPaired" flags
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
			roads1->graph[*ei]->fullyPaired = false;
		}
	}
}

/**
 * Precompute the tree structures starting from all the nodes.
 */
void RoadGraphDatabase::createTrees(RoadGraph* roads) {
	clearTrees();

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads->graph); vi != vend; ++vi) {
		if (!roads->graph[*vi]->valid) continue;

		BFSTree* tree = new BFSTree(roads, *vi, 5);
		trees[*vi] = tree;
	}
}

/**
 * Free all the allocated memory for the trees.
 */
void RoadGraphDatabase::clearTrees() {
	for (QMap<RoadVertexDesc, BFSTree*>::iterator it = trees.begin(); it != trees.end(); ++it) {
		delete it.value();
	}
}

float RoadGraphDatabase::computeRotationAngle(RoadGraph* roads1, RoadGraph* roads2, QMap<RoadVertexDesc, RoadVertexDesc>& map) {
	float total_angle = 0.0f;
	int count = 0;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads1->graph); ei != eend; ++ei) {
		if (!roads1->graph[*ei]->valid) continue;
		if (!roads1->graph[*ei]->fullyPaired) continue;

		RoadVertexDesc src1 = boost::source(*ei, roads1->graph);
		RoadVertexDesc tgt1 = boost::target(*ei, roads1->graph);

		RoadVertexDesc src2 = map[src1];
		RoadVertexDesc tgt2 = map[tgt1];

		total_angle += GraphUtil::diffAngle(roads2->graph[src2]->pt - roads2->graph[tgt2]->pt, roads1->graph[src1]->pt - roads1->graph[tgt1]->pt, false);
		count++;
	}

	return total_angle / (float)count;
}
