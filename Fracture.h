#pragma once
#include "src/MathGeoLib.h"
#include <list>

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

	typedef vector<float3> TRing;
	typedef vector<TRing> TRings;

protected:
	void GetPointsAndEdges(Polygon &Polygon2D);
	void GetPolyhedronFromEdges();
	void AddEdgesToMesh();
	void Dissolve();
	void CleanRings(TRing &OuterPoints, TRings &InnerRings);
public:
	FracturePolygon(Polygon &InPolygon, vector<Polygon> &FracturingPolygons, bool Side = true);
	Polyhedron &GetMesh() { return OutMesh; }
};
