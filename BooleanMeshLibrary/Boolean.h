#pragma once
#include <MathGeoLib.h>

using namespace std;
using namespace math;

typedef vector<LineSegment> Segments;
typedef vector<Line> Lines;

Polyhedron BooleanSum(Polyhedron InMeshA, Polyhedron InMeshB);