#include "ALGraph.h"

ALGraph::ALGraph(unsigned size) {
	vecVecAdjList.resize(size);
	nodes.resize(size);
}

ALGraph::~ALGraph(void) {

}

void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight) {
	AdjacencyInfo ai;

	ai.id = destination;
	ai.weight = weight;
	vecVecAdjList[source - 1].push_back(ai);
	nodes[source - 1].path.push_back(destination);

	std::sort(vecVecAdjList[source - 1].begin(), vecVecAdjList[source - 1].end(),
		[](AdjacencyInfo & lar, AdjacencyInfo & rar) {
		return lar.weight < rar.weight;
	});

	std::sort(nodes[source - 1].path.begin(), nodes[source - 1].path.end());

}

void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight){
	AddDEdge(node1, node2, weight);
	AddDEdge(node2, node1, weight);
}

unsigned ALGraph::weight(unsigned source, unsigned dest) const {
	return std::find_if(vecVecAdjList[source].begin(), vecVecAdjList[source ].end(),
		[&](AdjacencyInfo const & ar) { return (ar.id == dest); })->weight;
}

std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const {

	pq diq;

	*const_cast<unsigned *>(&nodes[start_node - 1].cost) = 0;
	unsigned i = 0;

	for (auto & dir : nodes) {

		if (i != start_node - 1)
			*const_cast<unsigned *>(&dir.cost) =
				std::numeric_limits<unsigned>::max();
		else
			diq.push(std::make_pair(i, const_cast<DijkstraInfo *>(&dir)));

		++i;
	}
	
	while (!diq.empty()) { 
		std::pair<unsigned, DijkstraInfo*> u = diq.top();
		diq.pop();
		for (auto & v : u.second->path) {
			unsigned alt = u.second->cost + weight(u.first, v);
			if (nodes[v-1].cost > alt) {
				*const_cast<unsigned*>(&nodes[v - 1].cost) = alt;
				diq.push(std::make_pair(v - 1, const_cast<DijkstraInfo *>(&nodes[v - 1]) ));
			}
		}
	}

	return nodes;
}

ALIST ALGraph::GetAList(void) const {
	return vecVecAdjList;
}