#pragma once
#include <list>
#include <vector>

using namespace std;

//point: x y z 
typedef struct point {
	float x;
	float y;
	float z;
}point;

// face: 
typedef struct face {
	int ind1;
	int ind2;
	int ind3;
}face;

// this is the parse obj file . all lines with token v are put into points .all lines with token f 
// are put into faces

typedef struct Mesh {
	vector<point *> *point;
	vector<face *> *face;
}Mesh;

namespace half_edge
{
	typedef long index_t;

	struct edge_t
	{
		index_t v[2];
		index_t& start() { return v[0];}
		const index_t& start() const { return v[0]; }

		index_t& end() { return v[1];}
		const index_t& end() const { return v[1]; }

		edge_t() {
			v[0] = v[1] = -1;
		}
	};
}