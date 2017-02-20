#pragma once
#include "src/MathGeoLib.h"

using namespace std;
typedef pair<int, int> TEdge;
typedef pair<float, int> TAngle;
typedef vector<TAngle> TNode;

class FracturePolygon {
	bool Side;
	Polygon &InPolygon; 
	vector<Polygon> &FracturingPolygons;
	Polyhedron OutMesh;

	vector<float3> Points;
	vector<Line> Lines;
	vector<TEdge> Edges;
	vector<TNode> Nodes;

protected:
	void GetPointsAndEdges(Polygon &Polygon2D);
	void GetPolyhedronFromEdges();
	void AddEdgesToMesh();
	void Dissolve();
public:
	FracturePolygon(Polygon &InPolygon, vector<Polygon> &FracturingPolygons, bool Side = true);
	Polyhedron &GetMesh() { return OutMesh; }
};
