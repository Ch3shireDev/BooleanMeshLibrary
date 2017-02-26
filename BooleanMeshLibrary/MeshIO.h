#pragma once
#include <chrono>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <vector>
#include <algorithm>
#define MATH_ENABLE_STL_SUPPORT
#include <MathGeoLib.h>
#include "WavefrontIO.h"


static double delta = 1e-12f;
static double delta2 = 1e-6;

using namespace math;
using namespace std;

void ReadOBJ(const char *name, Polyhedron &poly);
void WriteOBJ(const char *name, Polyhedron &poly);
void WriteOBJ(const string name, Polyhedron &poly);
void WriteOBJ(const string name, Polygon &poly);
ostream& operator<<(ostream &out, const Polyhedron::Face &face);
void FixPolyhedron(Polyhedron &poly);
void AddPolygonToMesh(Polygon &InPolygon, Polyhedron &Mesh);
void AddPolyhedronToMesh(Polyhedron &InPolyhedron, Polyhedron &Mesh);
void AddEdgeToMesh(LineSegment &Edge, Polyhedron &Mesh);


