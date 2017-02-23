#include "Boolean.h"
#include "Fracture.h"
#include "MeshIO.h"


using namespace std;


Polyhedron BooleanSum(Polyhedron InMeshA, Polyhedron InMeshB) {
	typedef vector<Polyhedron> Polyhedrons;
	typedef vector<Polygon> Polygons;
	Polyhedron Out;
	for (auto pp : { Polyhedrons({ InMeshA,InMeshB }), Polyhedrons({ InMeshB,InMeshA }) }) {
		auto p1 = pp[0], p2 = pp[1];
		Polygons P1 = p1.Faces(), P2 = p2.Faces();
		for (auto InPolygon : P1) {
			vector<Polygon> FracturingPolygons;
			for (auto pp2 : P2)	if (InPolygon.Intersects(pp2))FracturingPolygons.push_back(pp2);
			if (FracturingPolygons.size() == 0) {
				if (!p2.Contains(InPolygon))
					AddPolygonToMesh(InPolygon, Out);
				continue;
			}

			//most of computations happens here:
			AddPolyhedronToMesh(FracturePolygon(InPolygon, FracturingPolygons, true).GetMesh(), Out);

		}
	}
	return Out;
}
