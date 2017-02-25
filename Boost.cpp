#include "Fracture.h"
#include "MeshIO.h"

#include <deque>
#include <fstream>
#include <boost/geometry.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BPoint;
typedef bg::model::polygon<BPoint, false> BPolygon;
using bg::get;


void FracturePolygon::DividePolygon(TPolys &Polygons, TPolys::iterator it) {
	if (it->InnerRings.size() == 0)return;

	TRing &OuterRing = it->OuterRing;
	TRings &InnerRings = it->InnerRings;

	int nOuter = OuterRing.size();

	cout << OuterRing.size() << endl;
	cout << InnerRings.size() << endl;
	cout << InnerRings[0].size() << endl;

	int StartOuter = 1;
	int TargetOuter = 3;

	int InnerRingNum = 0;

	int nInner = InnerRings[0].size();
	int StartInner = 0;
	int TargetInner = 30;

	Polygon P0, P1;

	for (int i = StartOuter; i <= StartOuter + nOuter; i++) {

		P0.p.push_back(it->OuterRing[i%nOuter]);
	}
	for (int i = TargetOuter; i <= (TargetOuter > StartOuter ? StartOuter + nOuter : StartOuter); i++) 
		P1.p.push_back(it->OuterRing[i%nOuter]);
	for (int i = StartInner; i <= (TargetInner > StartInner ? TargetInner : TargetInner + nInner); i++)	
		P0.p.push_back(it->InnerRings[InnerRingNum][i]);
	for (int i = TargetInner; i <= (TargetInner > StartInner ? StartInner+nInner : StartInner); i++)
		P1.p.push_back(it->InnerRings[InnerRingNum][i%nInner]);

	WriteOBJ("a.obj", P0);
	WriteOBJ("b.obj", P1);

	exit(0);
}


void FracturePolygon::Dissolve() {
	if (OutMesh.NumFaces() < 2)return;

	Polyhedron PP;
	AddPolygonToMesh(InPolygon, PP);
	
	list<BPolygon> Polygons;

	for (auto p : OutMesh.Faces()) {
		BPolygon P;
		
		for (int i = 0; i < p.NumVertices()+1; i++) {
			auto v = InPolygon.MapTo2D(p.Vertex(i%p.NumVertices()));
			bg::append(P.outer(), BPoint(v.x, v.y));
		}

		Polygons.push_back(P);
	}

	
	
	for (auto it = Polygons.begin(); it != Polygons.end(); it++) {
		for (auto it2 = Polygons.begin(); it2 != it;) {
			vector<BPolygon> PPs;
			bg::union_(*it, *it2, PPs);
			auto it3 = it2++;
			if (PPs.size() != 1)continue;
			*it = PPs[0];
			Polygons.erase(it3);
		}
	}

	//for now we have a good polygon with hole. I need to find:
	//	1. Way to erase additional points from outer edges
	//	2. method to convert complex polygons (with inner rings) into non-weak simple polygons 
	//	(without inner rings, but also without self-touching edges)


	TPolys Polys;

	for (BPolygon p : Polygons) {
		TRing OuterPoints;
		for (BPoint pp : p.outer()) {
			double x = get<0>(pp);
			double y = get<1>(pp);
			float3 Point = InPolygon.MapFrom2D(float2(x, y));
			OuterPoints.push_back(Point);
		}
		OuterPoints.pop_back();
		TRings InnerRings;
		for (auto Inner : p.inners()) {
			TRing InnerPoints;
			for (BPoint pp : Inner) {
				double x = get<0>(pp);
				double y = get<1>(pp);
				float3 Point = InPolygon.MapFrom2D(float2(x, y));
				InnerPoints.push_back(Point);
			}
			InnerPoints.pop_back();
			InnerRings.push_back(InnerPoints);
		}
		TPolygon Poly;
		Poly.OuterRing = OuterPoints;
		Poly.InnerRings = InnerRings;
		Polys.push_back(Poly);
	}

	GetMeshFromRings(Polys);


}

void FracturePolygon::GetMeshFromRings(TPolys Polygons) {
	OutMesh = Polyhedron();
	for (auto &Poly : Polygons) CleanRings(Poly);

	for (auto it = Polygons.begin(); it != Polygons.end(); it++) {
		DividePolygon(Polygons, it); //dividing as long as InnerRings size > 0
		Polygon OutPolygon;
		for (auto v : it->OuterRing) OutPolygon.p.push_back(v);
		AddPolygonToMesh(OutPolygon, OutMesh);
	}

}

void FracturePolygon::CleanRings(TPolygon &Poly) {

	TRing &OuterPoints = Poly.OuterRing;
	TRings &InnerRings = Poly.InnerRings;

	TRing NewOuter;
	for (int i = 0; i < OuterPoints.size(); i++) {
		float3 Point = OuterPoints[i];
		bool flag = false;
		for (float3 p : InPolygon.p) {
			float d = p.Distance(Point);
			if (d < 1e-4) {
				NewOuter.push_back(Point);
				flag = true;
				break;
			}
		}
		if (flag)continue; //point is exactly the same as one of InPolygon points

		for (Polygon &P : FracturingPolygons) {
			float d = P.Distance(Point);
			if (d < 1e-4) {
				NewOuter.push_back(Point);
				break;
			}
		}
	}

	OuterPoints = NewOuter;
}