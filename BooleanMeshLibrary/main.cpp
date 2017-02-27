#include "Boolean.h"
#include "MeshIO.h"

using namespace std;

/*
	I have wrong results with concave polygons as faces. What's interesting it's not the fault
	of Dissolve method.
*/

int main(int argc, char** argv) {
	Polyhedron a0, b0, c0;
	ReadOBJ("../sphere.obj",a0);
	ReadOBJ("../cube.obj", b0);
	ReadOBJ("../shaped_cube.obj", c0);

	Polyhedron CoreMesh, p1, p2;
	
	for (int i = 0; i < 100; i++) {
		float3 r = float3(rand() / 2.f / RAND_MAX, rand() / 2.f / RAND_MAX, rand() / 2.f / RAND_MAX);
		p1 = a0, p2 = b0;
		p2.Translate(r);
		CoreMesh = BooleanSum(p1, p2);
		WriteOBJ("a" + to_string(i) + ".obj", CoreMesh);
		p1 = a0, p2 = c0;
		p2.Translate(r);
		CoreMesh = BooleanSum(p1, p2);
		WriteOBJ("b" + to_string(i) + ".obj", CoreMesh);
		cerr << i << endl;
	}
}