//---------------------------------------------------------------------------
#ifndef ALGRAPH_H
#define ALGRAPH_H
//---------------------------------------------------------------------------

/****************************************************************************/
/*!
\file   ALGraph.h
\author Ang Cheng Yong
\par    email: a.chengyong\@digipen.edu
\par    DigiPen login: a.chengyong
\par    Course: CS280
\par    Programming Assignment #5
\date   22/11/2016
\brief
This file contains the driver functions needed for adjacency list and dijkstra.
*/
/****************************************************************************/

#include <vector>
#include <algorithm>
#include <limits>
#include <queue> 

struct DijkstraInfo
{
  unsigned cost;
  std::vector<unsigned> path;
};

struct AdjacencyInfo
{
  unsigned id;
  unsigned weight;
};

typedef std::vector<std::vector<AdjacencyInfo> > ALIST;

class ALGraph
{
  public:
    ALGraph(unsigned size);
    ~ALGraph(void);
    void AddDEdge(unsigned source, unsigned destination, unsigned weight);
    void AddUEdge(unsigned node1, unsigned node2, unsigned weight);

    std::vector<DijkstraInfo> Dijkstra(unsigned start_node) const;
    ALIST GetAList(void) const;
        
  private:
	struct GEdge {
		GEdge();
		AdjacencyInfo * aiptr;
	};

	struct GNode {
		GNode();
		unsigned id;
		DijkstraInfo * di;
		std::vector<GEdge> edges;
	};

	void constructVertexs() const;
	unsigned weight(GNode & source, unsigned dest) const;
	GNode top(std::vector<GNode> & vr) const;
	bool visited(GNode & gr) const;

	mutable std::vector<DijkstraInfo> nodes;
	mutable std::vector<GNode> visitedNodes;
	mutable ALIST vecVecAdjList;
	mutable std::vector<GNode> vecVertex;
};

#endif
