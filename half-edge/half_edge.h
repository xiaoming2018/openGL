#pragma once
#include "data.h"
#include <vector>
#include <map>

namespace half_edge
{
	void faces_to_edges(vector<face *> *faces, vector<half_edge::edge_t> & edges_out);
	
	class half_edge_t
	{
	public:
		typedef long index_t;

		struct halfedge_t {
			index_t to_vertex;
			index_t face;
			index_t edge;

		};

		half_edge_t();
		~half_edge_t();

	private:

	};

	half_edge_t::half_edge_t()
	{
	}

	half_edge_t::~half_edge_t()
	{
	}

}
