//#include <boost/graph/edge_list.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <unordered_set>
#include <iostream>

#include "Graph.h"
#include "MeshIO.h"

using namespace std;

//using namespace boost;


struct Edge {
	int a, b;
	Edge() {}

	Edge(int a, int b) :a(a), b(b) {}
	int operator()(const Edge &e) const {
		return a + b;
	}
	bool operator==(const Edge& rhs) const {
		if (a == rhs.a&&b == rhs.b)return true;
		if (b == rhs.a&&a == rhs.b)return true;
		return false;
	}
};

void GraphFix(Polyhedron& Mesh) {
	//i can easily create graph from vertices + edges
	//after this i can join vertices into faces
	//how to get graph containing faces as nodes?

	//idea - for every node i can assign faces containing this node
	//after this I have easy way to get map of faces

	//ok. Every face has edges, in this case edge should always connect two faces. Therefore, if face doesn't connect other faces in number of it's edges/vertices, it's broken and it can be removed. I can repeat this operation as long as I have finally polygon which satisfies Euler's polyhedron formula

	//typedef boost::adjacency_list<vecS, vecS, undirectedS> Graph;


	//int N = (int)Mesh.NumVertices();
	//Graph G(N);
	//graph_traits<Graph>::vertex_iterator vi, vi_end;
	//tie(vi, vi_end) = vertices(G);

	//vector<vector<int>> VerticesFaces;
	//VerticesFaces.resize(N);
	//fill(VerticesFaces.begin(), VerticesFaces.end(), vector<int>());

	//for (int j = 0; j < Mesh.NumFaces(); j++) {
	//	auto &f = Mesh.f[j];
	//	int n = (int)f.v.size();
	//	for (int i = 0; i < n; i++) {
	//		auto t1 = vi, t2 = vi;
	//		advance(t1, f.v[i]);
	//		advance(t2, f.v[(i+1)%n]);
	//		add_edge(*t1, *t2, G);
	//		VerticesFaces[f.v[i]].push_back(j);
	//	}
	//}


	//graph_traits<Graph>::edge_iterator ei, ei_end;
	//tie(ei, ei_end) = edges(G);

	//typedef set<int> ESet;
	//typedef pair<Edge, ESet> EPair;
	//typedef	std::unordered_map<EPair::first_type, EPair::second_type, EPair::first_type> EMap;
	//EMap Map;

	//for (int i = 0; i < Mesh.NumFaces(); i++) {
	//	auto &Face = Mesh.f[i].v;
	//	auto n = Face.size();
	//	for (int j = 0; j < n; j++) {
	//		int a = Face[j];
	//		int b = Face[(j + 1) % n];
	//		auto it = Map.find(Edge(a, b));
	//		if (it == Map.end())Map.insert(EPair(Edge(a, b), ESet({ i })));
	//		else it->second.insert(i);
	//	}
	//}

	//typedef boost::adjacency_list<listS, listS, undirectedS, int, Edge> GraphList;
	//GraphList GFaces(Mesh.NumFaces());

	//graph_traits<GraphList>::vertex_iterator fi, fi_end;

	//tie(fi, fi_end) = vertices(GFaces);

	//int ii = 0;
	//for (auto it = fi; it != fi_end; it++) {
	//	GFaces[*it] = ii++;
	//}

	//for (auto it = Map.begin(); it != Map.end(); it++) {
	//	auto &Set = it->second;
	//	for(auto it2 = Set.begin();it2 != Set.end(); it2++)
	//		for (auto it3 = Set.begin(); it3 != it2; it3++) {
	//			auto iv1 = fi;
	//			auto iv2 = fi;
	//			advance(iv1, *it2);
	//			advance(iv2, *it3);
	//			add_edge(*iv1, *iv2, it->first, GFaces);
	//		}
	//}

	//graph_traits<GraphList>::edge_iterator e1, e2;
	//tie(e1, e2) = edges(GFaces);

	//graph_traits<GraphList>::out_edge_iterator e3, e4;

	//bool f2 = true;

	//int dbg = 0;

	//int jj = 0;

	//while (f2) {
	//	if (jj > 0)break;
	//	cerr << jj++ << endl;

	//	f2 = false;

	//	tie(fi, fi_end) = vertices(GFaces);

	//	for (auto it = fi; it != fi_end;) {
	//		tie(e3, e4) = out_edges(*it, GFaces);
	//		int ii = GFaces[*it];
	//		auto &Faces = Mesh.f[ii].v;

	//		std::unordered_set<Edge, Edge> USet;
	//		for (auto it2 = e3; it2 != e4; it2++)
	//			USet.insert(GFaces[*it2]);

	//		bool flag = false;
	//		auto n = Faces.size();
	//		for (int i = 0; i < n; i++) {
	//			auto it2 = USet.find(Edge(Faces[i], Faces[(i + 1) % n]));
	//			if (it2 == USet.end()) {
	//				flag = true;
	//				break;
	//			}
	//		}
	//		auto it2 = it;
	//		it++;
	//		if (flag) {

	//			tie(e3, e4) = out_edges(*it2, GFaces);
	//			for (auto ee = e3; ee != e4;) {
	//				auto ee2 = ee;
	//				ee++;
	//				remove_edge(*ee2, GFaces);
	//			}

	//			remove_vertex(*it2, GFaces);

	//			f2 = true;
	//		}
	//	}
	//}


	//Polyhedron Out;
	//for (auto it = fi; it != fi_end; it++)
	//	AddPolygonToMesh(Mesh.FacePolygon(GFaces[*it]),Out);

	//Mesh = Out;
	//FixPolyhedron(Mesh);

}