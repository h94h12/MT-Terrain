#include "glUtil.h"
#include "terrain.h"

#define Dot Vector3f



GLuint CreatePerlinCloud(unsigned indx, unsigned size);
void initSkyBox();
void initClouds();
void initOcean();
void initSun();

void drawSkyBoxDot(Dot d, double u, double v);
void drawSkyBox();
void drawClouds();
void drawCloudsReflection();
void drawOcean();

void drawSun();
void drawSunBloom();
Dot returnSunPos();
float returnSunRot();
void updateSunRot(float);
