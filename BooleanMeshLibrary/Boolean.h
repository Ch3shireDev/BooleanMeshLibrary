#pragma once
#define MATH_ENABLE_STL_SUPPORT
#include <MathGeoLib.h>

using namespace std;
using namespace math;

typedef vector<LineSegment> Segments;
typedef vector<Line> Lines;

Polyhedron BooleanSum(Polyhedron InMeshA, Polyhedron InMeshB);