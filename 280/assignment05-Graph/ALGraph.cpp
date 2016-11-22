/****************************************************************************/
/*!
\file   ALGraph.cpp
\author Ang Cheng Yong
\par    email: a.chengyong\@digipen.edu
\par    DigiPen login: a.chengyong
\par    Course: CS280
\par    Programming Assignment #2
\date   22/11/2016
\brief
This file contains the implememtation needed for adjacency list and dijkstra.
*/
/****************************************************************************/
#include "ALGraph.h"

/***************************************************************************/
/*!
\fn ALGraph::GNode::GNode()
\brief
Constructor of GNode
\return
none
*/
/***************************************************************************/
ALGraph::GNode::GNode() : id(0), di(0) {}

/***************************************************************************/
/*!
\fn ALGraph::GEdge::GEdge()
\brief
Constructor of GEdge
\return
none
*/
/***************************************************************************/
ALGraph::GEdge::GEdge() : aiptr(0) {}

/***************************************************************************/
/*!
\fn ALGraph::ALGraph(unsigned size)
\brief
Constructor of ALGraph
\return
none
*/
/***************************************************************************/
ALGraph::ALGraph(unsigned size) {
	vecVecAdjList.resize(size);
	nodes.resize(size);
}

/***************************************************************************/
/*!
\fn ALGraph::~ALGraph(void)
\brief
Destructor of ALGraph
\return
none
*/
/***************************************************************************/
ALGraph::~ALGraph(void) {}

/***************************************************************************/
/*!
\fn void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
\brief
Add a link between 2 nodes

\param source
where the link started from

\param destination
where the link ended at

\param weight
weight of the link

\return
none
*/
/***************************************************************************/
void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight) {
	AdjacencyInfo ai;

	ai.id = destination;
	ai.weight = weight;
	// add a destination for the current vertex
	vecVecAdjList[source - 1].push_back(ai);
	// sort the vertex
	std::sort(vecVecAdjList[source - 1].begin(), vecVecAdjList[source - 1].end(),
		[](AdjacencyInfo & lar, AdjacencyInfo & rar) {
		if (lar.weight != rar.weight)
			return lar.weight < rar.weight;
		else
			return lar.id < rar.id;
	});
}

/***************************************************************************/
/*!
\fn void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
\brief
Let both nodes know they are linked to each other

\param node1
where the link started from

\param node2
where the link ended at

\param weight
weight of the link

\return
none
*/
/***************************************************************************/
void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight){
	AddDEdge(node1, node2, weight);
	AddDEdge(node2, node1, weight);
}

/***************************************************************************/
/*!
\fn unsigned ALGraph::weight(GNode & source, unsigned dest) const
\brief
Get the weight of 2 connected nodes

\param source
node that connects from

\param dest
node that the source connects to

\return
none
*/
/***************************************************************************/
unsigned ALGraph::weight(GNode & source, unsigned dest) const {
	unsigned i = std::find_if(source.edges.begin(), source.edges.end(),
		[&](GEdge const & ar) { return (ar.aiptr->id == dest); })->aiptr->weight;
	return i;
}

/***************************************************************************/
/*!
\fn void ALGraph::constructVertexs() const
\brief
create vertexs from adjacency list and dijstrka info

\return
none
*/
/***************************************************************************/

void ALGraph::constructVertexs() const {
	vecVertex.resize(vecVecAdjList.size());

	for (unsigned i = 0; i < vecVecAdjList.size(); ++i) {
		vecVertex[i].edges.resize(vecVecAdjList[i].size());
		vecVertex[i].id = i;
		vecVertex[i].di = &nodes[i];
		for (unsigned j = 0; j < vecVecAdjList[i].size(); ++j)
			vecVertex[i].edges[j].aiptr = &vecVecAdjList[i][j];
	}
}

/***************************************************************************/
/*!
\fn ALGraph::GNode ALGraph::top(std::vector<GNode> & vr) const
\brief
Get the node with the shortest cost.

\param vr
vector of vertex

\return
Node with the shortest cost
*/
/***************************************************************************/

ALGraph::GNode ALGraph::top(std::vector<GNode> & vr) const {
	std::sort(vr.begin(), vr.end(),
		[&](GNode & ldr, GNode & rdr) {
		if (ldr.di->cost != rdr.di->cost)
			return ldr.di->cost < rdr.di->cost;
		else
			return ldr.id < rdr.id;
	});

	return *vr.begin();
}

/***************************************************************************/
/*!
\fn bool ALGraph::visited(GNode & gr) const
\brief
check whether a node is visited

\param gr
the node to check

\return
whether the node is visited
*/
/***************************************************************************/

bool ALGraph::visited(GNode & gr) const {
	return (std::find_if(visitedNodes.begin(), visitedNodes.end(),
		[&](GNode & gr_) { return (gr.id == gr_.id); }) != visitedNodes.end());
}

/***************************************************************************/
/*!
\fn std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
\brief
the dijkstra shortest path algorithm

\param start_node
starting node

\return
the shortest path consisting of nodes
*/
/***************************************************************************/

std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const {

	constructVertexs();

	std::vector<GNode> tmpVv; // the visited Q

	for (auto & gv : vecVertex) // set initial cost to infinity
		gv.di->cost = std::numeric_limits<unsigned>::max();
	
	vecVertex[start_node - 1].di->cost = 0; // except starting vertex to 0
	// the starting node will be part of the path
	tmpVv.push_back(vecVertex[start_node - 1]); 
	vecVertex[start_node - 1].di->path.push_back(start_node);

	while(!tmpVv.empty()){
		GNode u = top(tmpVv);
		tmpVv.erase(tmpVv.begin());
		
		if (visited(u)) // skip if  visited
			continue;

		for (auto & v : u.edges) {
			int dest = v.aiptr->id;
			unsigned alt = u.di->cost + weight(u, dest);
			if (vecVertex[dest - 1].di->cost > alt) {
				// adjust cost if shorter path found 
				// and change path
				vecVertex[dest - 1].di->cost = alt;
				nodes[dest - 1].path = u.di->path;
				nodes[dest - 1].path.push_back(dest);
				tmpVv.push_back(vecVertex[dest - 1]);

			}
		
		}
		// add visited node to visited Q
		visitedNodes.push_back(u);
	}

	return nodes;
}

/***************************************************************************/
/*!
\fn ALIST ALGraph::GetAList(void) const
\brief
get the data from adjacency list

\return
the adjacency list
*/
/***************************************************************************/
ALIST ALGraph::GetAList(void) const {
	return vecVecAdjList;
}