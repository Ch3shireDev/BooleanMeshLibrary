#pragma once
#include <MathGeoLib.h>
#include "WavefrontIO.h"
#include <iostream>

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


