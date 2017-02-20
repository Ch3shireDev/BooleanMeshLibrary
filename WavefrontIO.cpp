#include "WavefrontIO.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <algorithm>
#include <fstream>

#include <iostream>

using namespace std;

void  Wavefront::WriteOBJ(const char *name, vector<double> &Vertices, vector<vector<int>> &Indices) {
	ofstream file(name);
	for (int i = 0; i < Vertices.size() / 3; i++) {
		double x = Vertices[3 * i];
		double y = Vertices[3 * i + 1];
		double z = Vertices[3 * i + 2];
		file << "v " << x << " " << y << " " << z << endl;
	}
	for (auto v : Indices) {
		file << "f ";
		for (int j = 0; j < v.size(); j++) {
			int i = v[j];
			file << i + 1 << " ";
		}
		file << endl;
	}
	file.close();
}

void  Wavefront::ReadOBJ(const char *name, vector<double> &Vertices, vector<vector<int>> &Indices) {
	tinyobj::attrib_t Vert;
	vector<tinyobj::shape_t> Shape;
	vector<tinyobj::material_t> Mat;
	string err;
	tinyobj::LoadObj(&Vert, &Shape, &Mat, &err, name, NULL, false);

	for (int i = 0; i < Vert.vertices.size(); i++) {
		double v = Vert.vertices[i];
		Vertices.push_back(v);
	}

	if (Shape.size() == 0) { cerr << "No input detected!" << endl; return; }

	auto NF = Shape[0].mesh.num_face_vertices;
	auto Ind = Shape[0].mesh.indices;

	int k = 0;

	for (int i = 0; i < NF.size(); i++) {
		vector<int> Polygon;
		for (int j = 0; j < (int)NF[i]; j++) {
			tinyobj::index_t it = Ind[k++];
			Polygon.push_back(it.vertex_index);
		}
		Indices.push_back(Polygon);
	}

}