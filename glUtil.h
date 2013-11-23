#ifndef GLUTIL
#define GLUTIL

#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <tr1/unordered_map>
#include <cmath>
#include <list>
#include <stdlib.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include "Eigen/Dense"
#include "Eigen/StdVector"

using namespace Eigen;
using namespace std::tr1;
using namespace std;

#define MIN_FLOAT 1e-37f
#define limit 11
#define epsilon 0.0f

typedef pair<Vector3f*, Vector3f*> PairVecP;

extern bool wireframe;

class Grid;

namespace std {
	bool operator==(const PairVecP &l, const PairVecP &r);
}

namespace std {
	namespace tr1 {
		bool operator==(const PairVecP &l, const PairVecP &r); 
		template<>
		struct hash<PairVecP> {
			std::size_t operator()(PairVecP const &key) const;
		};
	}
}

class Tetrahedron {
public:
	Vector3f *v1, *v2, *v3, *v4;
	Vector3f cutpoints[6];
	int CPC;
	Grid *grid;
	Tetrahedron(Vector3f *v1, Vector3f *v2, Vector3f *v3, Vector3f *v4, Grid *grid);
	void draw();
};

class Edge {
public:
	Vector3f *v1, *v2;
	Edge(Vector3f *v1, Vector3f *v2);
	Edge();
	bool findCP(float (*f)(Vector3f), Vector3f &intersection);
};

class Grid {
public:
	float (*func)(Vector3f);
	vector<Tetrahedron> tets;
	unordered_map< pair<Vector3f*, Vector3f*>, Edge> edges;
	vector<Vector3f> vertices;
	Vector3f center, step, gridMax;
	Grid(Vector3f center, Vector3f step, Vector3f gridMax, float (*func)(Vector3f));
	void draw();
};

#endif