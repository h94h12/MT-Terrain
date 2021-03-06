#ifndef GLUTIL
#define GLUTIL

#include <vector>
#include <list>  
#include <functional>
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits.h>
#include <string.h>
#include <sstream>
#include <utility>
#include <vector>
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
#include "textures/lodepng.h"
#include "Eigen/Dense"
#include "Eigen/StdVector"

using namespace Eigen;
using namespace std;

#define MIN_FLOAT 1e-37f
#define limit 11
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define epsilon 0.0001f
#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF

#define CIRCLE 0
#define BUTTERFLY 1
#define CYCLOID 2
#define LINKS 3
#define AMPERSAND 4
#define CRUCIFORM 5
#define CLOVER 6

class Grid;
class Triangle;
class Viewport;
class Island; 

void initTerrainTextures();

typedef list<GLuint> ShaderObjList;
typedef vector<Triangle/*, Eigen::aligned_allocator<Eigen::Vector3f> */> vector_tri;

extern bool wireframe;
extern vector_tri tris;
extern Viewport viewport;

GLuint AddTextureToOpenGL(unsigned w, unsigned h, void * ptr);
GLuint LoadTextureFromPNG(const char * filename, unsigned = 255);


class Viewport {
  public:
    int w, h; // width and height
};

class Tetrahedron {
public:
	Vector3f *v1, *v2, *v3, *v4;
	int CPC;
	Grid *grid;
	Tetrahedron(Vector3f *v1, Vector3f *v2, Vector3f *v3, Vector3f *v4);
	void addTriangles(vector_tri *triangles, float (*func)(Vector3f));
};

class Grid { 
	Vector3i numSteps;
public:
	vector<Vector3f, Eigen::aligned_allocator<Eigen::Vector3f> > vertices;
	void addTriangles(vector_tri *triangles, float (*func)(Vector3f));
	Grid(Vector3f center, Vector3f step, Vector3f gridMax);
};

class Triangle {
public:
	Triangle(Vector3f v1, Vector3f v2, Vector3f v3);
	void draw();

private:
	Vector3f v1, v2, v3;
};

class Island{
public:
	//centered at (x, c)
	Island(int, float, float);
	int shape;
	float xc; 
	float yc; 
};

// SKYDOME

/*

class CubeMapTex{
public:
	CubeMapTex(const string& dir, 
		const string& PXFilename, 
		const string& NXFilename, 
		const string& PYFilename, 
		const string& NYFilename, 
		const string& PZFilename,
		const string& NZFilename);

	~CubeMapTex();
	bool load();
	void bind(GLenum TextureUnit);

private:
	string fileNames[6];
	GLuint m_textureObj;
};


class Technique {
public: 
	Technique();
	virtual ~Technique();
	virtual bool Init();
	void Enable();
protected:
	bool AddShader(GLenum Type, const char* pText);
	bool Finalize();
	GLint GetUniformLocation(const char* pUniformName);
private:
	GLuint m_shaderProj;
	ShaderObjList m_shaderObjList;
};

class SkyboxTechnique : public Technique {
public:
 
    SkyboxTechnique();
 
    virtual bool Init();
 
    void SetWVP(const Matrix4f& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
 
private:
 
    GLuint m_WVPLocation;
    GLuint m_textureLocation;
};
**/




#endif
