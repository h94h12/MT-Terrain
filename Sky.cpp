#include "Sky.h"

GLuint t_SkyBoxTop,t_SkyBoxBottom;
GLuint t_Clouds1, t_Clouds2;
float skyboxMax = 20.0;

GLuint CreatePerlinCloud(unsigned indx, unsigned size) {
    unsigned width = size, height = size;
    std::vector<unsigned char> image;
    image.reserve(width * height * 4);
     
    HeightMap hm = HeightMap(width); 
    if (indx == 1) {
        hm.addPerlinNoise(2); 
        hm.addPerlinNoise(7); 
        hm.addPerlinNoise(13); 
        hm.smoothen();
        hm.erode(5);
    } else {
        hm.addPerlinNoise(3);    
        hm.addPerlinNoise(12); 
        hm.addPerlinNoise(25); 
        hm.erode(2);
        hm.smoothen();

    }

    float mmax = -9999;
    float mmin = 9999;
    for(unsigned y = 0; y < height; y++) {
        for(unsigned x = 0; x < width; x++) {
             float pn = ((hm).heights[x*height + y]);
             mmax = std::max(pn, mmax);
             mmin = std::min(pn, mmin);
        }
    }
    float easeDist = height/5;
    for(unsigned y = 0; y < height; y++) {
        for(unsigned x = 0; x < width; x++) {
            //std::cout << "(*hm).heights[x*height + y]" << (hm).heights[x*height + y] << std::endl;
            float pn = ((hm).heights[x*height + y]-mmin)/(mmax - mmin);
            pn = sqrtf(pn*255);
            float pn2 = 255 - ((pn-4)*25);
            
            image[4 * width * y + 4 * x + 0] = 255.0-pn;
            image[4 * width * y + 4 * x + 1] = 255.0-pn;
            image[4 * width * y + 4 * x + 2] = 255.0-(pn*0.7);
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
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); // select modulate to mix texture with color for shading

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 1 ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 1 ? GL_REPEAT : GL_CLAMP );
    
    
    void * ptr = &image[0];
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

    return texture;
    //return AddTextureToOpenGL(width, height, &image[0]);
}

void initializeSkyBox() {
    cout << "initializeSkyBox() " << endl;
    //t_SkyBoxTop = LoadTextureFromPNG("textures/sky_temp.png");
    t_SkyBoxTop = LoadTextureFromPNG("textures/sm64_ocean.png");
    
    
    
    t_Clouds1 = CreatePerlinCloud(1, 512);
    t_Clouds2 = CreatePerlinCloud(2, 512);
    /* 
     * 
     * for some reason, a second call to CreatePerlinCloud is glitchy and results in 
     * random NaN's... even if it's the same arguments as the first, successful time
     * 
     * 
    */
    //t_Clouds = LoadTextureRAW("texture/transp.png", 1);
    //t_SkyBoxTop = LoadTextureRAW("texture/test5.raw", 1);
}

void drawSkyBoxDot(Dot d, double u, double v) {
    glTexCoord2f(u, v); glVertex3f(d(0), d(1), d(2));
}

void drawSkyBox() {
    float a = skyboxMax;

    Dot sky_bur = Dot(-a, a, a);
    Dot sky_bul = Dot( a, a, a);
    Dot sky_bdr = Dot(-a,-a, a); 
    Dot sky_bdl = Dot( a,-a, a);

    Dot sky_fur = Dot(-a, a,-a);
    Dot sky_ful = Dot( a, a,-a);
    Dot sky_fdr = Dot(-a,-a,-a);  
    Dot sky_fdl = Dot( a,-a,-a);
    
    
    glDisable(GL_LIGHTING);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, t_SkyBoxTop);
    
    
    glBegin(GL_QUADS); // Back
        drawSkyBoxDot(sky_bur, 1.0, 0.0);
        drawSkyBoxDot(sky_bul, 0.0, 0.0);
        drawSkyBoxDot(sky_bdl, 0.0, 1.0);
        drawSkyBoxDot(sky_bdr, 1.0, 1.0);

    glEnd(); glBegin(GL_QUADS); // Front
        drawSkyBoxDot(sky_fur, 1.0, 0.0);
        drawSkyBoxDot(sky_ful, 0.0, 0.0);
        drawSkyBoxDot(sky_fdl, 0.0, 1.0);
         drawSkyBoxDot(sky_fdr, 1.0, 1.0);
    
    glEnd(); glBegin(GL_QUADS); // Left
        drawSkyBoxDot(sky_bul, 1.0, 0.0);
        drawSkyBoxDot(sky_ful, 0.0, 0.0);
        drawSkyBoxDot(sky_fdl, 0.0, 1.0);
        drawSkyBoxDot(sky_bdl, 1.0, 1.0);
        
     glEnd(); glBegin(GL_QUADS); // Right
        drawSkyBoxDot(sky_bur, 1.0, 0.0);
        drawSkyBoxDot(sky_fur, 0.0, 0.0);
        drawSkyBoxDot(sky_fdr, 0.0, 1.0);
        drawSkyBoxDot(sky_bdr, 1.0, 1.0);
    
    glEnd(); glBegin(GL_QUADS); // Down
        drawSkyBoxDot(sky_bdl, 1.0, 0.0);
        drawSkyBoxDot(sky_fdl, 0.0, 0.0);
        drawSkyBoxDot(sky_fdr, 0.0, 1.0);
        drawSkyBoxDot(sky_bdr, 1.0, 1.0);

    glEnd(); glBegin(GL_QUADS); // Up
        drawSkyBoxDot(sky_bul, 1.0, 0.0);
        drawSkyBoxDot(sky_ful, 0.0, 0.0);
        drawSkyBoxDot(sky_fur, 0.0, 1.0);
        drawSkyBoxDot(sky_bur, 1.0, 1.0);
        
    glEnd();
    glEnable(GL_LIGHTING);
    glDisable( GL_TEXTURE_2D );
}

void drawClouds() {
    float a = skyboxMax;

    Dot sky_bur = Dot(-a, a, a);
    Dot sky_bul = Dot( a, a, a);
    Dot sky_bdr = Dot(-a,-a, a); 
    Dot sky_bdl = Dot( a,-a, a);

    Dot sky_fur = Dot(-a, a,-a);
    Dot sky_ful = Dot( a, a,-a);
    Dot sky_fdr = Dot(-a,-a,-a);  
    Dot sky_fdl = Dot( a,-a,-a);
    
    Dot cloudlevel1 = Dot(0, a*0.8, 0);
    Dot cloudlevel2 = Dot(0, a*0.7, 0);
    Dot cloudlevel3 = Dot(0, a*0.6, 0);
    Dot cloudlevel4 = Dot(0, a*0.5, 0);
    
    // Hand-wavy depth sorting.... with transparency, the order they are rendered matters, but that 
    // requires depth sorting.  Instead of that, we assume that since they are the same shade, with 
    // pretty much olnly variations in alpha, that it'll work well enough.
    
    
    glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, t_Clouds1);
    glBegin(GL_QUADS); // Up Clouds
        drawSkyBoxDot(sky_bul - cloudlevel1, 1.0, 0.0);
        drawSkyBoxDot(sky_ful - cloudlevel1, 0.0, 0.0);
        drawSkyBoxDot(sky_fur - cloudlevel1, 0.0, 1.0);
        drawSkyBoxDot(sky_bur - cloudlevel1, 1.0, 1.0);  glEnd();
    
    //t_SkyBoxTop
    glBindTexture(GL_TEXTURE_2D, t_Clouds2);
    glBegin(GL_QUADS); // Up Clouds
        drawSkyBoxDot(sky_bul - cloudlevel2, 1.0, 0.0);
        drawSkyBoxDot(sky_ful - cloudlevel2, 0.0, 0.0);
        drawSkyBoxDot(sky_fur - cloudlevel2, 0.0, 1.0);
        drawSkyBoxDot(sky_bur - cloudlevel2, 1.0, 1.0);glEnd();
        
    glBegin(GL_QUADS); // Up Clouds
        drawSkyBoxDot(sky_bul - cloudlevel3, 1.0, 0.0);
        drawSkyBoxDot(sky_ful - cloudlevel3, 0.0, 0.0);
        drawSkyBoxDot(sky_fur - cloudlevel3, 0.0, 1.0);
        drawSkyBoxDot(sky_bur - cloudlevel3, 1.0, 1.0);glEnd();
        
    glBegin(GL_QUADS); // Up Clouds
        drawSkyBoxDot(sky_bul - cloudlevel4, 1.0, 0.0);
        drawSkyBoxDot(sky_ful - cloudlevel4, 0.0, 0.0);
        drawSkyBoxDot(sky_fur - cloudlevel4, 0.0, 1.0);
        drawSkyBoxDot(sky_bur - cloudlevel4, 1.0, 1.0);glEnd();
    


    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable( GL_TEXTURE_2D );
    
}