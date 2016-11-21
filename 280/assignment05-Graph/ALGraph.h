//---------------------------------------------------------------------------
#ifndef ALGRAPH_H
#define ALGRAPH_H
//---------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <limits>
#include <queue> 
#include <iostream>

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

	struct sortNodes {
		bool operator()(std::pair<unsigned, DijkstraInfo*> const & ldr, 
						std::pair<unsigned, DijkstraInfo*> const & rdr) {
			if (ldr.second->cost != rdr.second->cost)
				return ldr.second->cost > rdr.second->cost;
			else
				return ldr.first > rdr.first;
		}
	};

	typedef std::priority_queue<std::pair<unsigned, DijkstraInfo*>,
		std::vector<std::pair<unsigned, DijkstraInfo*>>, sortNodes> pq;

	unsigned weight(unsigned source, unsigned dest) const;
	std::pair<unsigned, DijkstraInfo*> top(std::vector<std::pair<unsigned, DijkstraInfo*>> & vr) const;
	void constructVertexs() const;
	// An EXAMPLE of some other classes you may want to create and 
    // implement in ALGraph.cpp
	struct GEdge {
		GEdge() :aiptr(0) {}
		AdjacencyInfo * aiptr;
	};

	struct GNode {
		GNode() :prev(0), id(0), cost(0) {}
		GNode * prev;
		unsigned id;
		unsigned cost;
		std::vector<GEdge> edges;
		unsigned weight(unsigned dest) {
			return std::find_if(edges.begin(), edges.end(),
				[&](GEdge & er) { return (er.aiptr->id == dest); })->aiptr->weight;
		}
	};

    struct AdjInfo
    {
      GNode *node;
      unsigned weight;
      unsigned cost;
      AdjInfo();
      bool operator<(const AdjInfo& rhs) const;
      bool operator>(const AdjInfo& rhs) const;
    };
    
    // Other private fields and methods
	std::vector<DijkstraInfo> nodes;
	mutable ALIST vecVecAdjList;
	mutable std::vector<GNode> vecVertex;
};

#endif
