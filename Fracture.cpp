#include "Fracture.h"
#include "MeshIO.h"
#include <algorithm>
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>
#include <cmath>

using namespace std;
typedef pair<int, int> TEdge;
typedef pair<float, int> TAngle;
typedef vector<TAngle> TNode;



void FracturePolygon::AddEdgesToMesh() {

	Polyhedron &Mesh = OutMesh;
	Mesh.v = Points;

	float3 Normal = InPolygon.NormalCCW();
	vector<int> UsedEdges;
	UsedEdges.resize(Edges.size(), 0);
	bool flag = true;
	int j = 0;
	while (flag) {
		TEdge e;
		int k;
		for (int i = 0; i < Edges.size(); i++) {
			if (UsedEdges[i] == 3)continue;
			e = Edges[i];
			if (UsedEdges[i] == 2)swap(e.first, e.second);
			k = i;
			flag = false;
			break;
		}
		if (flag)break;
		vector<int> Indices;
		Indices.push_back(e.first);
		while (e.second != Indices[0]) {
			Indices.push_back(e.second);
			if (UsedEdges[k] == 0) {
				if (e == Edges[k])UsedEdges[k] = 2;
				else UsedEdges[k] = 1;
			}
			if (UsedEdges[k] > 0)UsedEdges[k] = 3;
			auto &Nod = Nodes[e.second];
			int i = 0;
			while (Nod[i].second != e.first)i++;
			e.first = e.second;
			int N = (int)Nod.size();
			e.second = Nod[(i + 1) % N].second;
		}
		Polyhedron::Face f;
		for (auto t = Indices.rbegin(); t != Indices.rend(); t++)f.v.push_back(*t);
		Mesh.f.push_back(f);
		if (Mesh.FaceNormal(Mesh.NumFaces() - 1).Dot(Normal) < 0)Mesh.f.pop_back(); //don't touch
		else {
			auto p = Mesh.FacePolygon(Mesh.NumFaces() - 1);
			bool flag = false;
			auto Center = p.CenterPoint();
			float dist = 9999;
			for (auto fp : FracturingPolygons) {
				float d = fp.Distance(Center);
				if (d < 1e-8)continue;
				if (d < dist) {
					dist = d;
					flag = fp.PlaneCCW().IsOnPositiveSide(Center);
				}
			}
			if (!flag)Mesh.f.pop_back();
		}
		flag = true;
	}

	Dissolve();
	//exit(0);
}

















void FracturePolygon::GetPolyhedronFromEdges() {

	for (auto &p : Points)
		p = InPolygon.MapFrom2D(p.xy());
	Nodes.reserve(Points.size());
	for (int i = 0; i < Points.size(); i++) {
		TNode Node;
		for (auto p : Edges) {
			if (p.first != i && p.second != i)continue;
			if (p.second == i)swap(p.first, p.second);
			float2 a = InPolygon.MapTo2D(Points[p.first]), b = InPolygon.MapTo2D(Points[p.second]);
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			TAngle e(atan2(dy, dx), p.second);
			Node.push_back(e);
		}
		sort(Node.begin(), Node.end());
		Nodes.push_back(Node);
	}

	AddEdgesToMesh();
}




FracturePolygon::FracturePolygon(Polygon &InPolygon, vector<Polygon> &FracturingPolygons, bool Side):
	InPolygon(InPolygon),
	FracturingPolygons(FracturingPolygons),
	Side(Side)
{
	Polygon Polygon2D;
	for (int i = 0; i < InPolygon.NumVertices(); i++) {
		auto p = InPolygon.MapTo2D(i);
		Polygon2D.p.push_back(float3(p.x, p.y, 0));
	}
	for (int i = 0; i < Polygon2D.NumEdges(); i++)Lines.push_back(Polygon2D.Edge(i).ToLine());
	auto P1 = InPolygon.PlaneCCW();
	auto N1 = P1.normal;
	auto d1 = P1.d;
	for (auto &P : FracturingPolygons) {
		auto P2 = P.PlaneCCW();
		auto N2 = P2.normal;
		auto N3 = N1.Cross(N2).Normalized();
		auto d2 = P2.d;
		float n1n2 = N1.Dot(N2);
		float3 p3 = d1*N1 + (d2 - n1n2*d1) / (1 - n1n2*n1n2)*(N2 - n1n2*N1);
		p3 -= p3.Dot(N3)*N3;
		float2 pos2d = InPolygon.MapTo2D(p3);
		float2 dir2d = InPolygon.MapTo2D(InPolygon.p[0] + N3);
		Lines.push_back(Line(float3(pos2d.x, pos2d.y, 0), float3(dir2d.x, dir2d.y, 0)));
	}
	GetPointsAndEdges(Polygon2D);
	GetPolyhedronFromEdges();
}

void FracturePolygon::GetPointsAndEdges(Polygon &Polygon2D) {

	auto n = Lines.size();
	vector<vector<float>> tValues;
	tValues.resize(n);


	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			float3 v1 = Lines[i].dir, v2 = Lines[j].dir;
			float3 p1 = Lines[i].pos, p2 = Lines[j].pos;
			float t1 = (p2 - p1).Cross(v2).z / v1.Cross(v2).z;
			float t2 = (p1 - p2).Cross(v1).z / v2.Cross(v1).z;
			if (!(isfinite(t1) && isfinite(t2)))continue;
			float3 r = p1 + t1*v1;
			float3 r2 = Lines[i].GetPoint(t1);
			if (Polygon2D.Distance(r) > 1e-6f)continue;
			Points.push_back(r);

			tValues[i].push_back(t1);
			tValues[j].push_back(t2);
		}
	}

	for (int i = 0; i < Lines.size(); i++) {
		auto &t = tValues[i];
		sort(t.begin(), t.end());
		for (int j = 1; j < t.size(); j++) {
			float d = t[j] - t[j - 1];
			if (d < 1e-6f) t.erase(t.begin() + j--);
		}

		for (int j = 0; j < t.size(); j++) {
			float3 r = Lines[i].GetPoint(t[j]);
			bool flag = true;
			for (auto &q : Points)if (q.Distance(r) < 1e-6) { flag = false; break; }
			if (flag)Points.push_back(r);
		}
	}

	for (int i = 0; i < Lines.size(); i++) {
		auto &L = Lines[i];
		for (int j = 1; j < tValues[i].size(); j++) {
			float t0 = tValues[i][j - 1], t1 = tValues[i][j];
			auto p0 = L.GetPoint(t0), p1 = L.GetPoint(t1);
			int k0 = -1, k1 = -1;
			for (int k = 0; k < Points.size(); k++) {
				if (k0 == -1)if (Points[k].Distance(p0) < 1e-6)k0 = k;
				if (k1 == -1)if (Points[k].Distance(p1) < 1e-6)k1 = k;
				if (k0 != -1 && k1 != -1)break;
			}
			if (k0 > k1)swap(k0, k1);
			if (k0 == k1)continue;
			Edges.push_back(TEdge(k0, k1));
		}
	}
	sort(Edges.begin(), Edges.end());
	auto it = unique(Edges.begin(), Edges.end());
	Edges.resize(distance(Edges.begin(), it));
}