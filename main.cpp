#include "Boolean.h"
#include "MeshIO.h"

using namespace std;


typedef vector<Polygon> Polygons;
typedef vector<Polyhedron> Polyhedrons;


int main(int argc, char** argv) {
	Polyhedron p0, c0;
	ReadOBJ("sphere.obj", p0);
	ReadOBJ("cube.obj", c0);

	Polyhedron CoreMesh, p1, p2;
	
	for (int i = 0; i < 100; i++) {
		float3 r = float3(rand() / 2.f / RAND_MAX, rand() / 2.f / RAND_MAX, rand() / 2.f / RAND_MAX);
		p1 = p0, p2 = c0;
		p2.Translate(r);
		CoreMesh = BooleanSum(p1, p2);
		WriteOBJ(to_string(i) + ".obj", CoreMesh);
		cerr << i << endl;
	}
}