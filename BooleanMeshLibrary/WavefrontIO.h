#pragma once
#include <vector>

using std::vector;

namespace Wavefront {

	void WriteOBJ(const char* name, vector<double> &Vertices, vector<vector<int>> &Indices);
	void ReadOBJ(const char* name, vector<double> &Vertices, vector<vector<int>> &Indices);

}