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

	std::sort(vecVecAdjList[source - 1].begin(), vecVecAdjList[source - 1].end(),
		[](AdjacencyInfo & lar, AdjacencyInfo & rar) {
		if (lar.weight != rar.weight)
			return lar.weight < rar.weight;
		else
			return lar.id < rar.id;
	});
}

void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight){
	AddDEdge(node1, node2, weight);
	AddDEdge(node2, node1, weight);
}

unsigned ALGraph::weight(unsigned source, unsigned dest) const {
	return std::find_if(vecVecAdjList[source].begin(), vecVecAdjList[source].end(),
		[&](AdjacencyInfo const & ar) { return (ar.id == dest); })->weight;
}

std::pair<unsigned, DijkstraInfo*> ALGraph::top(std::vector<std::pair<unsigned, DijkstraInfo*>> & vr) const {
	std::sort(vr.begin(), vr.end(),
		[&](std::pair<unsigned, DijkstraInfo*> & ldr, std::pair<unsigned, DijkstraInfo*> & rdr) {
		if (ldr.second->cost != rdr.second->cost)
			return ldr.second->cost < rdr.second->cost;
		else
			return ldr.first < rdr.first;
	});

	return *vr.begin();
}

std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const {

	std::vector<std::pair<unsigned, DijkstraInfo*>> diq;

	*const_cast<unsigned *>(&nodes[start_node - 1].cost) = 0;
	//const_cast<DijkstraInfo*>(&nodes[start_node - 1])->path.push_back(start_node);
	unsigned i = 0;

	for (auto & dir : nodes) {

		if (i != start_node - 1)
			*const_cast<unsigned *>(&dir.cost) =
				std::numeric_limits<unsigned>::max();
		else
			diq.push_back(std::make_pair(i, const_cast<DijkstraInfo *>(&dir)));

		++i;
	}
	
	while (!diq.empty()) { 
		std::pair<unsigned, DijkstraInfo*> u = top(diq);
		diq.erase(diq.begin());
		//diq.pop();

		unsigned i = 0;
		for (auto & v : vecVecAdjList[u.first]) {
			unsigned alt = u.second->cost + weight(u.first, v.id);
			if (nodes[v.id - 1].cost > alt) {
				i = v.id;
				*const_cast<unsigned*>(&nodes[v.id - 1].cost) = alt;
				diq.push_back(std::make_pair(v.id - 1, const_cast<DijkstraInfo *>(&nodes[v.id - 1])));
			
			}
		
		}
		//uvr.push_back(i);
	}

	return nodes;
}

ALIST ALGraph::GetAList(void) const {
	return vecVecAdjList;
}