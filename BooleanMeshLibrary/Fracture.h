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

	struct TPolygon {
		TRing OuterRing;
		TRings InnerRings;
	};

	typedef list<TPolygon> TPolys;

protected:
	void GetPointsAndEdges(Polygon &Polygon2D);
	void GetPolyhedronFromEdges();
	void AddEdgesToMesh();
	void Dissolve();
	void CleanRings(TPolygon &Poly);
	void GetMeshFromRings(TPolys Polygons);
	void DividePolygon(TPolys &Polygons, TPolys::iterator Iterator);
public:
	FracturePolygon(Polygon &InPolygon, vector<Polygon> &FracturingPolygons, bool Side = true);
	Polyhedron &GetMesh() { return OutMesh; }
};