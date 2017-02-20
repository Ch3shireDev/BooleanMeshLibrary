#include "MeshIO.h"
#include <unordered_set>

//static double d2 = 1e-12;



struct hashf {
	double operator()(const float3& v) const {
		return v.x*1e3;
	}

	float operator()(const float2& v) const {
		return v.x;
	}

};

struct vecf {
	float x, y, z;
	double operator()(const vecf &rhs) const {
		return (double)rhs.x;
	}

	float3 operator ()() const {
		return float3(x, y, z);
	}

	bool operator==(const vecf &rhs) const {
		if (operator()().Distance(rhs()) < 1e-6)return true;
		else return false;
	}

	vecf(float3 vec) :x(vec.x), y(vec.y), z(vec.z) {}
	vecf() {}

};

void FixPolyhedron(Polyhedron &InMesh) {

	Polyhedron Out, Temp;

	int m0 = 1, m1 = 1, m2 = 1;
	int n0 = 0, n1 = 0, n2 = 0;



	for (int i = 0; i < InMesh.NumFaces(); i++) {
		auto P = InMesh.FacePolygon(i);

		AddPolygonToMesh(P,Temp);
	}

	unordered_map<vecf, int, vecf> Map;


		int n = 0;
		for (auto v : Temp.v) {
			n = (int)Map.size();
			Map[v];
			if (Map.size() != n) { 
				Out.v.push_back(v);
				Map[v] = n; 
			}
		}

		for (auto f : Temp.f) {

			Polyhedron::Face F;

			for (auto i : f.v) {
				auto p = Temp.v[i];
				int j = Map[p];
				F.v.push_back(j);
			}

			Out.f.push_back(F);
		}

		InMesh = Out;

}