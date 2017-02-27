#include "MeshIO.h"

#include <cstdlib>
#include <algorithm>

static double delta = 1e-12f;
static double delta2 = 1e-6;

void WriteOBJ(const string name, Polygon &poly) {
	Polyhedron Out;
	AddPolygonToMesh(poly, Out);
	WriteOBJ(name, Out);
}

void AddPolygonToMesh(Polygon &InPolygon, Polyhedron &Mesh) {
	int n = (int)Mesh.v.size();
	Polyhedron::Face f;
	for (auto v : InPolygon.p) {
		Mesh.v.push_back(v);
		f.v.push_back(n++);
	}
	Mesh.f.push_back(f);
}

void AddPolyhedronToMesh(Polyhedron &InPolyhedron, Polyhedron &Mesh) {
	for (int i = 0; i < InPolyhedron.NumFaces(); i++)
		AddPolygonToMesh(InPolyhedron.FacePolygon(i), Mesh);
}

void AddEdgeToMesh(LineSegment &Edge, Polyhedron &Mesh) {
	Mesh.v.push_back(Edge.a);
	Mesh.v.push_back(Edge.b);
}

void ReadOBJ(const char *name, Polyhedron &poly) {
	vector<double> Vertices;
	vector<vector<int>> Indices;
	Wavefront::ReadOBJ(name, Vertices, Indices);

	for (int i = 0; i < Vertices.size() / 3; i++)
		poly.v.push_back(POINT_VEC(
		(float)Vertices[3 * i],
			(float)Vertices[3 * i + 1],
			(float)Vertices[3 * i + 2])
		);

	for (auto &v : Indices) {
		Polyhedron::Face f0;
		f0.v = v;
		poly.f.push_back(f0);
	}
}

void WriteOBJ(const string name, Polyhedron &poly) {
	WriteOBJ(name.c_str(), poly);
}

void WriteOBJ(const char *name, Polyhedron &poly) {

	vector<double> Vertices;
	vector<vector<int>> Indices;

	for (int i = 0; i < poly.NumVertices(); i++) {
		auto v = poly.Vertex(i);
		Vertices.push_back(v.x);
		Vertices.push_back(v.y);
		Vertices.push_back(v.z);
	}

	for (int i = 0; i < poly.NumFaces(); i++) {
		Polyhedron::Face f = poly.f[i];
		Indices.push_back(poly.f[i].v);
	}

	Wavefront::WriteOBJ(name, Vertices, Indices);
}

ostream& operator<<(ostream &out, const math::Polyhedron::Face &face) {
	for (int i : face.v)
		out << i << " ";
	return out;
}