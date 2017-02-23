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

void FracturePolygon::CleanRings(TRing &OuterPoints, TRings &InnerRings) {
	
	//cout << "Another: " << endl;

	//cout << OuterPoints.size() << endl;

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

		for (Polygon P : FracturingPolygons) {
			float d = P.Distance(Point);
			if (d < 1e-4) {
				NewOuter.push_back(Point);
				break;
			}
		}
	}
	//cout << NewOuter.size() << endl;
	OuterPoints = NewOuter;
	return;

	//for (TRing &Ring : InnerRings) {
	//	cout << "Before: " << Ring.size() << endl;
	//	TRing NewRing;
	//	for(int i=0;i<Ring.size();i++){
	//		float3 Point = Ring[i];
	//		for (Polygon &P : FracturingPolygons) {
	//			bool flag = false;
	//			for (int i = 0; i < P.NumEdges(); i++) {
	//				LineSegment e = P.Edge(i);
	//				float d = e.Distance(Point); //it didn't catch all points
	//				if (d < 1e-5f) {
	//					flag = true;
	//					NewRing.push_back(Point);
	//					break;
	//				}
	//			}
	//			if (flag)break;
	//		}
	//	}
	//	Ring = NewRing;
	//	cout << "After: " << Ring.size() << endl;
	//}
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


	Polyhedron Out;

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
		CleanRings(OuterPoints, InnerRings);

		Polygon OutPolygon;
		for (auto v : OuterPoints) {
			OutPolygon.p.push_back(v);
		}
		
		AddPolygonToMesh(OutPolygon, Out);


		//cout << polygon.NumVertices() << endl;
		//Polyhedron Out;
		//AddPolygonToMesh(polygon, Out);
		//WriteOBJ("a.obj", Out);
		//polygon.p.clear();
		//for (auto v : InnerRings[0]) {
			//polygon.p.push_back(v);
		//}
		//Out = Polyhedron();
		//AddPolygonToMesh(polygon, Out);
		//WriteOBJ("b.obj", Out);

	}

	//WriteOBJ("out.obj", Out);
	/*static int i = 0;
	char s1[64], s2[64];
	sprintf(s1, "%03d.obj", i);
	sprintf(s2, "%03da.obj", i);
*/
	//for (auto p : FracturingPolygons)AddPolygonToMesh(p, OutMesh);

	//WriteOBJ(s1, OutMesh);
	OutMesh = Out;
	//WriteOBJ(s2, OutMesh);
	//i++;
	//exit(0);
}

