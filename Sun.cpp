#include "Sky.h"


float skyboxMax2 = 25.0;

float sunRot, sunSpin1, sunSpin2, sunX, sunY, sunZ, sunDistance;
GLuint sunTexture, sunRaysTexture;
Dot sunPosDot;

Dot sun_ul = Dot(-skyboxMax2*0.5, -skyboxMax2*0.5, 0);
Dot sun_ur = Dot(-skyboxMax2*0.5,  skyboxMax2*0.5, 0);
Dot sun_dr = Dot(skyboxMax2*0.5,   skyboxMax2*0.5, 0);
Dot sun_dl = Dot(skyboxMax2*0.5,  -skyboxMax2*0.5, 0);


void initSun() {
    sunRot = 3.14159;
    sunSpin1 = 0;
    sunSpin2 = 0;
    sunDistance = skyboxMax2*0.85;
    sunTexture = LoadTextureFromPNG("textures/sun.png");
    sunRaysTexture = LoadTextureFromPNG("textures/sun_retry5.png");
    
    sunX = 0;//cos(sunRot);
    sunY = sin(-sunRot)*sunDistance;
    sunZ = cos(-sunRot)*sunDistance;
    sunPosDot = Dot(sunX, sunY, sunZ);
}





void updateSunRot(float sr) {
    sunRot = sr;
    while (sunRot > 3.14159*2) {
        sunRot -= 3.14159*2;
    }
    while (sunRot < 0) {
        sunRot += 3.14159*2;
    }
}

void drawSun() {
    updateSunRot(sunRot + 0.01);
    sunSpin1 += 0.9;
    sunSpin2 += 0.53;
    while (sunSpin1 > 360) {
        sunSpin1 -= 360;
    }
    while (sunSpin2 > 360) {
        sunSpin2 -= 360;
    }
    float sunRotInDegrees = sunRot*180/3.141592;
    
    
    glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    


    glRotatef(sunRotInDegrees, 1, 0, 0);

    // main sun texture
   /* glBindTexture(GL_TEXTURE_2D, sunTexture);
    glBegin(GL_QUADS);
        drawSkyBoxDot(sun_ul + Dot(0, 0, sunDistance*0.98), 1.0, 0.0);
        drawSkyBoxDot(sun_ur + Dot(0, 0, sunDistance*0.98), 0.0, 0.0);
        drawSkyBoxDot(sun_dr + Dot(0, 0, sunDistance*0.98), 0.0, 1.0);
        drawSkyBoxDot(sun_dl + Dot(0, 0, sunDistance*0.98), 1.0, 1.0); glEnd(); 
 */
    glRotatef(sunSpin1, 0, 0, 1);////
    
        glBindTexture(GL_TEXTURE_2D, sunRaysTexture);
        glBegin(GL_QUADS);
            drawSkyBoxDot(sun_ul + Dot(0, 0, sunDistance*0.99), 1.0, 0.0);
            drawSkyBoxDot(sun_ur + Dot(0, 0, sunDistance*0.99), 0.0, 0.0);
            drawSkyBoxDot(sun_dr + Dot(0, 0, sunDistance*0.99), 0.0, 1.0);
            drawSkyBoxDot(sun_dl + Dot(0, 0, sunDistance*0.99), 1.0, 1.0); glEnd(); 
     glRotatef(-sunSpin1, 0, 0, 1);////
     
     
    glRotatef(-sunSpin2, 0, 0, 1);////
    
        glBindTexture(GL_TEXTURE_2D, sunRaysTexture);
        glBegin(GL_QUADS);
            drawSkyBoxDot(sun_ul + Dot(0, 0, sunDistance*1.00), 1.0, 0.0);
            drawSkyBoxDot(sun_ur + Dot(0, 0, sunDistance*1.00), 0.0, 0.0);
            drawSkyBoxDot(sun_dr + Dot(0, 0, sunDistance*1.00), 0.0, 1.0);
            drawSkyBoxDot(sun_dl + Dot(0, 0, sunDistance*1.00), 1.0, 1.0); glEnd(); 
     glRotatef(+sunSpin2, 0, 0, 1);////
     
     
     
     
    glRotatef(-sunRotInDegrees, 1, 0, 0);    



   


    
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable( GL_TEXTURE_2D );
}

float returnSunRot() {
    return sunRot;
}



Dot returnSunPos() {
    //sunX = 0;//cos(sunRot);
    //sunY = sin(-sunRot)*sunDistance;
    //unZ = cos(-sunRot)*sunDistance;
    return Dot(0, sin(-sunRot)*sunDistance, cos(-sunRot)*sunDistance);
}






void drawSunReflection() {
    
    
    
}





