#include "Sky.h"

GLuint t_SkyBoxTop;


unsigned cloudLayerCount = 5;
std::vector<GLuint> t_Clouds;
std::vector<float> cloudsRot;
std::vector<Dot> cloudlevels;


float skyboxMax = 15.0;
float a = skyboxMax;



float sunRot, sunX, sunY, sunZ;
GLuint sunTexture;
float sunDistance;
Dot sunPosDot;


Dot sky_bur = Dot(-a, a, a);
Dot sky_bul = Dot( a, a, a);
Dot sky_bdr = Dot(-a,-a, a); 
Dot sky_bdl = Dot( a,-a, a);

Dot sky_fur = Dot(-a, a,-a);
Dot sky_ful = Dot( a, a,-a);
Dot sky_fdr = Dot(-a,-a,-a);  
Dot sky_fdl = Dot( a,-a,-a);







// openGL texturing based off tutorial from http://www.nullterminator.net/gltexture.html
GLuint CreatePerlinCloud(unsigned indx, unsigned size, bool blue) {
    unsigned width = size, height = size;
    std::vector<unsigned char> image;
    image.reserve(width * height * 4);
     
    HeightMap hm = HeightMap(width, indx*123456789); 
    //cout << "pre-rendering cloud layer : " << indx << endl;

    hm.addPerlinNoise(2 + (indx%2));
    hm.smoothen();
    hm.addPerlinNoise(4 + (indx%5)); 
    hm.addPerlinNoise(7 + (indx%2)); 

    hm.addPerlinNoise(9 + (indx%10)); 
    hm.addPerlinNoise(16 + (indx%20)); 
    
    for (unsigned i = 0; i < (indx % 6) + 5; i++) hm.erode((indx % 5) + 2);
    hm.smoothen();
    

    float mmax = -9999999;
    float mmin = 9999999;
    for(unsigned y = 0; y < height; y++) {
        for(unsigned x = 0; x < width; x++) {
             float pn = ((hm).heights[x*height + y]);
             mmax = std::max(pn, mmax);
             mmin = std::min(pn, mmin);
        }
    }
   // cout << "mmax: " << mmax << ", mmin:" << mmin << endl;
    float easeDist = height/5;
    for(unsigned y = 0; y < height; y++) {
        for(unsigned x = 0; x < width; x++) {
            float pn = ((hm).heights[x*height + y]-mmin)/(mmax - mmin);

            pn = sqrt(pn*255);

           
            float pn2 = 255 - ((pn-4)*25);
            

            if (blue) {
                pn2 *= 0.2;
                image[4 * width * y + 4 * x + 0] = std::max(0.0, 55.0-pn*8);
                image[4 * width * y + 4 * x + 1] = std::max(0.0, 55.0-pn*8);
                image[4 * width * y + 4 * x + 2] = 255.0-(pn*0.7);
            } else {
                pn2 *= 3.0/(float) cloudLayerCount; // so if there's a lot of clouds, each one is less visible
                image[4 * width * y + 4 * x + 0] = 255.0-pn;
                image[4 * width * y + 4 * x + 1] = 255.0-pn;
                image[4 * width * y + 4 * x + 2] = 255.0-(pn*0.7);
            }
            image[4 * width * y + 4 * x + 3] = std::min((float) 255,  (std::max((float) 0, pn2)));
            
            // ease out
            if (x < easeDist) image[4 * width * y + 4 * x + 3] *= x/easeDist;
            if ((width-x) < easeDist) image[4 * width * y + 4 * x + 3] *= (width-x)/easeDist;
            
            if (y < easeDist) image[4 * width * y + 4 * x + 3] *= y/easeDist;
            if ((height-y) < easeDist) image[4 * width * y + 4 * x + 3] *= (height-y)/easeDist;
        }
    }
  
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    void * ptr = &image[0];
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

    return texture;
    //return AddTextureToOpenGL(width, height, &image[0]);
}

void initSkyBox() {
    cout << "initSkyBox() " << endl;
    t_SkyBoxTop = LoadTextureFromPNG("textures/skybox_texture.png");

}

void drawSkyBoxDot(Dot d, double u, double v) {
    glTexCoord2f(u, v); glVertex3f(d(0), d(1), d(2));
}

void drawSkyBox() {
    
    //cout << "t_SkyBoxTop" << t_SkyBoxTop << endl;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_SkyBoxTop);
    
    
    glBegin(GL_QUADS); // Back
        drawSkyBoxDot(sky_bur, 0.75,  1.0/3.0);
        drawSkyBoxDot(sky_bul, 1.00, 1.0/3.0);
        drawSkyBoxDot(sky_bdl, 1.00, 2.0/3.0);
        drawSkyBoxDot(sky_bdr, 0.75, 2.0/3.0);

    glEnd(); glBegin(GL_QUADS); // Front
        drawSkyBoxDot(sky_fur, .5, 1.0/3.0);
        drawSkyBoxDot(sky_ful, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fdl, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_fdr, .5, 2.0/3.0);
    
    glEnd(); glBegin(GL_QUADS); // Left
        drawSkyBoxDot(sky_bul, 0.0, 1.0/3.0);
        drawSkyBoxDot(sky_ful, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fdl, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_bdl, 0.0, 2.0/3.0);
        
     glEnd(); glBegin(GL_QUADS); // Right
        drawSkyBoxDot(sky_bur, 0.75, 1.0/3.0);
        drawSkyBoxDot(sky_fur, 0.5, 1.0/3.0);
        drawSkyBoxDot(sky_fdr, 0.5, 2.0/3.0);
        drawSkyBoxDot(sky_bdr, 0.75, 2.0/3.0);
    
    glEnd(); glBegin(GL_QUADS); // Down
        drawSkyBoxDot(sky_bdl, 0.25, 1.0);
        drawSkyBoxDot(sky_fdl, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_fdr, 0.5, 2.0/3.0);
        drawSkyBoxDot(sky_bdr, 0.5, 1.0);

    glEnd(); glBegin(GL_QUADS); // Up
        drawSkyBoxDot(sky_bul, 0.25, 0.0/3.0);
        drawSkyBoxDot(sky_ful, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fur, 0.5, 1/3.0);
        drawSkyBoxDot(sky_bur, 0.5, 0.0/3.0);
        
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable( GL_TEXTURE_2D );
}


void initClouds() {
    cout << "initClouds() " << endl;
    
    t_Clouds.resize(cloudLayerCount);
    cloudsRot.resize(cloudLayerCount);
    cloudlevels.resize(cloudLayerCount);
    
    
    for (unsigned i = 0; i < cloudLayerCount; i++) {
        t_Clouds[i] = CreatePerlinCloud(i, 512, false);
        
        cloudsRot[i] = ((rand()%1024)/1024)*3.141592*2;
        
        
        float rat = (float) i / (float) cloudLayerCount;
        cloudlevels[i] = Dot(0, a*(0.5 + (0.4*rat)), 0); // range [0.5 to 0.9)

    }


}



void drawClouds() {
    // Hand-wavy depth sorting.... with transparency, the order they are rendered matters, but that 
    // requires depth sorting.  Instead of that, we assume that since they are the same shade, with 
    // pretty much olnly variations in alpha, that it'll work well enough.
    
    
    glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    
    for (unsigned i = 0; i < cloudLayerCount; i++) {
        glRotatef(cloudsRot[i], 0, 1, 0);
        cloudsRot[i] += ((i%4) - 1.0)*2.0*0.02;
        
        glBindTexture(GL_TEXTURE_2D, t_Clouds[i]);
        glBegin(GL_QUADS);

        drawSkyBoxDot(sky_bul - cloudlevels[i], 1.0, 0.0);
        drawSkyBoxDot(sky_ful - cloudlevels[i], 0.0, 0.0);
        drawSkyBoxDot(sky_fur - cloudlevels[i], 0.0, 1.0);
        drawSkyBoxDot(sky_bur - cloudlevels[i], 1.0, 1.0);
        glEnd();
        
        glRotatef(-cloudsRot[i], 0, 1, 0); // undo that rotation!
    }


    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
}










GLuint oceanTexture;
//GLuint sandTexture;
GLuint wavesTexture;
double waveCount;

void initOcean(){
    oceanTexture = LoadTextureFromPNG("textures/water.png");
    //sandTexture = LoadTextureFromPNG("textures/water.png");
    //wavesTexture0 = CreatePerlinCloud(0, 512, true);
    wavesTexture = LoadTextureFromPNG("textures/water.png", 70);
    waveCount = 0;
}



void drawOcean(){

    waveCount += 0.001;
    Dot oceanlevel = Dot(0, a, 0);
    Dot ol = Dot(0, 0.01, 0);

    
    glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    
    
    glBindTexture(GL_TEXTURE_2D, oceanTexture);
    glBegin(GL_QUADS); 
        drawSkyBoxDot(sky_bul - oceanlevel, 1.0, 0.0);
        drawSkyBoxDot(sky_ful - oceanlevel, 0.0, 0.0);
        drawSkyBoxDot(sky_fur - oceanlevel, 0.0, 1.0);
        drawSkyBoxDot(sky_bur - oceanlevel, 1.0, 1.0); glEnd();
      
    glBindTexture(GL_TEXTURE_2D, wavesTexture);
    glBegin(GL_QUADS); 
        drawSkyBoxDot(sky_bul - oceanlevel + ol, 1.0 + waveCount, 0.0 + waveCount);
        drawSkyBoxDot(sky_ful - oceanlevel + ol, 0.0 + waveCount, 0.0 + waveCount);
        drawSkyBoxDot(sky_fur - oceanlevel + ol, 0.0 + waveCount, 1.0 + waveCount);
        drawSkyBoxDot(sky_bur - oceanlevel + ol, 1.0 + waveCount, 1.0 + waveCount); glEnd();
        
    glBindTexture(GL_TEXTURE_2D, wavesTexture);
    glBegin(GL_QUADS); 
        drawSkyBoxDot(sky_bul - oceanlevel + ol + ol, 1.0 - waveCount*0.5, 0.0);
        drawSkyBoxDot(sky_ful - oceanlevel + ol + ol, 0.0 - waveCount*0.5, 0.0);
        drawSkyBoxDot(sky_fur - oceanlevel + ol + ol, 0.0 - waveCount*0.5, 1.0);
        drawSkyBoxDot(sky_bur - oceanlevel + ol + ol, 1.0 - waveCount*0.5, 1.0); glEnd();
        
    glBindTexture(GL_TEXTURE_2D, wavesTexture);
    glBegin(GL_QUADS); 
        drawSkyBoxDot(sky_bul - oceanlevel + ol + ol + ol, 1.0 + waveCount*0.5, 0.0);
        drawSkyBoxDot(sky_ful - oceanlevel + ol + ol + ol, 0.0 + waveCount*0.5, 0.0);
        drawSkyBoxDot(sky_fur - oceanlevel + ol + ol + ol, 0.0 + waveCount*0.5, 1.0);
        drawSkyBoxDot(sky_bur - oceanlevel + ol + ol + ol, 1.0 + waveCount*0.5, 1.0); glEnd();

      
        
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}






void initSun() {
    sunRot = 0;
    sunDistance = skyboxMax*0.9;
    sunTexture = LoadTextureFromPNG("textures/sun.png");
    
    sunX = 0;//cos(sunRot);
    sunY = sin(-sunRot)*sunDistance;
    sunZ = cos(-sunRot)*sunDistance;
    sunPosDot = Dot(sunX, sunY, sunZ);
}


Dot sun_ul = Dot(-skyboxMax/2, -skyboxMax/2, 0);
Dot sun_ur = Dot(-skyboxMax/2,  skyboxMax/2, 0);
Dot sun_dr = Dot(skyboxMax/2,   skyboxMax/2, 0);
Dot sun_dl = Dot(skyboxMax/2,  -skyboxMax/2, 0);


void drawSun() {
    sunRot += 0.05; 
    float sunRotInDegrees = sunRot*180/3.141592;
    
    
    glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    


    glRotatef(sunRotInDegrees, 1, 0, 0);

    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glBegin(GL_QUADS);

        
        drawSkyBoxDot(sun_ul + Dot(0, 0, sunDistance), 1.0, 0.0);
        drawSkyBoxDot(sun_ur + Dot(0, 0, sunDistance), 0.0, 0.0);
        drawSkyBoxDot(sun_dr + Dot(0, 0, sunDistance), 0.0, 1.0);
        drawSkyBoxDot(sun_dl + Dot(0, 0, sunDistance), 1.0, 1.0);
     glEnd(); 

    glRotatef(-sunRotInDegrees, 1, 0, 0);


    
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable( GL_TEXTURE_2D );
}

Dot returnSunPos() {
    //sunX = 0;//cos(sunRot);
    //sunY = sin(-sunRot)*sunDistance;
    //unZ = cos(-sunRot)*sunDistance;
    return Dot(0, sin(-sunRot)*sunDistance, cos(-sunRot)*sunDistance);
}










