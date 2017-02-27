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


//one hole case for now.

void FracturePolygon::DividePolygon(TPolys &Polygons, TPolys::iterator it) {
	if (it->InnerRings.size() == 0)return;

	TRing &OuterRing = it->OuterRing;
	TRings &InnerRings = it->InnerRings;

	if (InnerRings.size() > 1) {
		cout << "Error - number of holes larger than one" << endl;
		exit(0);
	}

	int nOuter = OuterRing.size();
	int nInner = InnerRings[0].size();
	int InnerRingNum = 0;

	int StartOuter = -1;
	int TargetOuter = -1;

	int StartInner = -1;
	int TargetInner = -1;

	float dmin = 0;
	for (int i = 0; i < nOuter; i++) {
		float3 p1 = OuterRing[i];
		for (int j = 0; j < nInner; j++) {
			float3 p2 = InnerRings[InnerRingNum][j];
			float d = p1.Distance(p2);
			if (d == 0)continue;
			d = 1 / d;
			if (d > dmin) {
				dmin = d;
				StartOuter = i;
				TargetInner = j;
			}
		}
	}

	if (StartOuter < 0 || TargetInner < 0){
		cout << "Error! No points found" << endl;
		exit(0);
	}

	dmin = 0;
	float3 p1 = OuterRing[StartOuter];
	for (int i = 0; i < nOuter; i++) {
		if (i == StartOuter)continue;
		float3 p2 = OuterRing[i];
		float d = p1.Distance(p2);
		if (d == 0)continue;
		if (d > dmin) {
			dmin = d;
			TargetOuter = i;
		}
	}

	dmin = 0;
	p1 = OuterRing[TargetOuter];

	for (int i = 0; i < nInner; i++) {
		if (i == TargetInner)continue;
		float3 p2 = InnerRings[InnerRingNum][i];
		float d = p1.Distance(p2);
		if (d == 0)continue;
		d = 1 / d;
		if (d > dmin) {
			dmin = d;
			StartInner = i;
		}
	}


	Polygon P0, P1;

	if (StartOuter < TargetOuter) {
		for (int i = StartOuter; i <= TargetOuter; i++)
			P0.p.push_back(OuterRing[i%nOuter]);
		for (int i = TargetOuter; i <= StartOuter + nOuter; i++)
			P1.p.push_back(OuterRing[i%nOuter]);
	}
	else {
		for (int i = StartOuter; i <= TargetOuter + nOuter; i++)
			P0.p.push_back(OuterRing[i%nOuter]);
		for (int i = TargetOuter; i <= StartOuter; i++)
			P1.p.push_back(OuterRing[i%nOuter]);
	}

	if (StartInner < TargetInner) {
		for (int i = StartInner; i <= TargetInner; i++)
			P0.p.push_back(InnerRings[InnerRingNum][i%nInner]);
		for (int i = TargetInner; i <= StartInner+nInner; i++)
			P1.p.push_back(InnerRings[InnerRingNum][i%nInner]);
	}
	else {
		for (int i = StartInner; i <= TargetInner+nInner; i++)
			P0.p.push_back(InnerRings[InnerRingNum][i%nInner]);
		for (int i = TargetInner; i <= StartInner; i++)
			P1.p.push_back(InnerRings[InnerRingNum][i%nInner]);
	}


	InnerRings.clear();
	OuterRing = P0.p;

	TPolygon PP;
	PP.OuterRing = P1.p;

	Polygons.push_back(PP);

}


void FracturePolygon::Dissolve() {
	if (OutMesh.NumFaces() < 2)return;
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