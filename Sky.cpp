#include "Sky.h"

GLuint t_SkyBoxTop;


float skyboxMax = 24.0;
unsigned cloudLayerCount = 8;
std::vector<GLuint> t_Clouds;
std::vector<float> cloudsRot;
std::vector<Dot> cloudlevels;



float a = skyboxMax;




Dot sky_bur = Dot(-a, a, a);
Dot sky_bul = Dot( a, a, a);
Dot sky_bdr = Dot(-a,-a, a); 
Dot sky_bdl = Dot( a,-a, a);

Dot sky_fur = Dot(-a, a,-a);
Dot sky_ful = Dot( a, a,-a);
Dot sky_fdr = Dot(-a,-a,-a);  
Dot sky_fdl = Dot( a,-a,-a);

Dot skybox_offset = Dot(0, a*0.4, 0);







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
            pn2 *= 6.0/(float) cloudLayerCount; // so if there's a lot of clouds, each one is less visible
            image[4 * width * y + 4 * x + 0] = 255.0;
            image[4 * width * y + 4 * x + 1] = 255.0;
            image[4 * width * y + 4 * x + 2] = 255.0;
        
            image[4 * width * y + 4 * x + 3] = std::min((float) 255,  (std::max((float) 0, pn2)));
            
            // ease out
            if (x < easeDist) image[4 * width * y + 4 * x + 3] *= x/easeDist;
            if ((width-x) < easeDist) image[4 * width * y + 4 * x + 3] *= (width-x)/easeDist;
            
            if (y < easeDist) image[4 * width * y + 4 * x + 3] *= y/easeDist;
            if ((height-y) < easeDist) image[4 * width * y + 4 * x + 3] *= (height-y)/easeDist;
        }
    }
  
    return AddTextureToOpenGL(width, height, &image[0]);
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
    
    //glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_SkyBoxTop);
    
    
    glBegin(GL_QUADS); // Back
        drawSkyBoxDot(sky_bur + skybox_offset, 0.75,  1.0/3.0);
        drawSkyBoxDot(sky_bul + skybox_offset, 1.00, 1.0/3.0);
        drawSkyBoxDot(sky_bdl + skybox_offset, 1.00, 2.0/3.0);
        drawSkyBoxDot(sky_bdr + skybox_offset, 0.75, 2.0/3.0);

    glEnd(); glBegin(GL_QUADS); // Front
        drawSkyBoxDot(sky_fur + skybox_offset, .5, 1.0/3.0);
        drawSkyBoxDot(sky_ful + skybox_offset, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fdl + skybox_offset, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_fdr + skybox_offset, .5, 2.0/3.0);
    
    glEnd(); glBegin(GL_QUADS); // Left
        drawSkyBoxDot(sky_bul + skybox_offset, 0.0, 1.0/3.0);
        drawSkyBoxDot(sky_ful + skybox_offset, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fdl + skybox_offset, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_bdl + skybox_offset, 0.0, 2.0/3.0);
        
     glEnd(); glBegin(GL_QUADS); // Right
        drawSkyBoxDot(sky_bur + skybox_offset, 0.75, 1.0/3.0);
        drawSkyBoxDot(sky_fur + skybox_offset, 0.5, 1.0/3.0);
        drawSkyBoxDot(sky_fdr + skybox_offset, 0.5, 2.0/3.0);
        drawSkyBoxDot(sky_bdr + skybox_offset, 0.75, 2.0/3.0);
    
    glEnd(); glBegin(GL_QUADS); // Down
        drawSkyBoxDot(sky_bdl + skybox_offset, 0.25, 1.0);
        drawSkyBoxDot(sky_fdl + skybox_offset, 0.25, 2.0/3.0);
        drawSkyBoxDot(sky_fdr + skybox_offset, 0.5, 2.0/3.0);
        drawSkyBoxDot(sky_bdr + skybox_offset, 0.5, 1.0); // down isn't visible, so cut out

    glEnd(); glBegin(GL_QUADS); // Up
        drawSkyBoxDot(sky_bul + skybox_offset, 0.25, 0.0/3.0);
        drawSkyBoxDot(sky_ful + skybox_offset, 0.25, 1.0/3.0);
        drawSkyBoxDot(sky_fur + skybox_offset, 0.5, 1/3.0);
        drawSkyBoxDot(sky_bur + skybox_offset, 0.5, 0.0/3.0);
        
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
    //glDisable(GL_LIGHTING);
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



void drawCloudsReflection() {


}






GLuint oceanTexture;
//GLuint sandTexture;
GLuint wavesTexture;
double waveCount;

void initOcean(){
    oceanTexture = LoadTextureFromPNG("textures/water.png", 90);
    //sandTexture = LoadTextureFromPNG("textures/water.png");
    //wavesTexture0 = CreatePerlinCloud(0, 512, true);
    wavesTexture = LoadTextureFromPNG("textures/water.png", 90);
    waveCount = 0;
}



void drawOcean(){

    waveCount += 0.001;
    Dot oceanlevel = Dot(0, a-0.003, 0);
    Dot ol = Dot(0, 0.002, 0);

    
    glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_LIGHTING);
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
        drawSkyBoxDot(sky_bul - oceanlevel + ol + ol + ol, 1.0 + waveCount*0.9, 0.0);
        drawSkyBoxDot(sky_ful - oceanlevel + ol + ol + ol, 0.0 + waveCount*0.9, 0.0);
        drawSkyBoxDot(sky_fur - oceanlevel + ol + ol + ol, 0.0 + waveCount*0.9, 1.0);
        drawSkyBoxDot(sky_bur - oceanlevel + ol + ol + ol, 1.0 + waveCount*0.9, 1.0); glEnd();

      
        
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}




