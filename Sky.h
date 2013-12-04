#include "glUtil.h"
#include "terrain.h"

#define Dot Vector3f

GLuint CreatePerlinCloud(unsigned indx, unsigned size);
void initializeSkyBox();
void drawSkyBoxDot(Dot d, double u, double v);
void drawSkyBox();
void drawClouds();