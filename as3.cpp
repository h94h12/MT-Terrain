#include "glUtil.h"
#include "terrain.h"
#include "Sky.h"
#include "Camera.h"

//****************************************************
// Some Classes and Global Standalone Functions
//****************************************************

#define GRID_X_MAX 10
#define GRID_Y_MAX 10
#define GRID_Z_MAX 10

#define STEP_X 0.1  //change to 0.05 later
#define STEP_Y 0.1
#define STEP_Z 0.1
#define pi 3.14159

#define HEIGHTMAP_SIZE (int)(2 * GRID_X_MAX * (1/STEP_X))

HeightMap h;

//GLuint grassTexture;

Vector3f gridMax = Vector3f(GRID_X_MAX, GRID_Y_MAX, GRID_Z_MAX); 
Vector3f stepsize = Vector3f(STEP_X, STEP_Y, STEP_Z); 

void initializeDensityFunction(){
    h = HeightMap(HEIGHTMAP_SIZE, rand()); 
    h.addPerlinNoise(20); 
    h.perturb(10, 3); 
    for(int i = 0; i < 10; i++) h.erode(16); 
    h.smoothen(); 
}

float closeCurve(float x, float y){
    int a = 6; 
    float b = x*x + y*y; 
    //return pow(x, 4) + 2*x*x*y*y + pow(y, 4) - pow(x, 3) + 3*x*y*y - GRID_X_MAX*1.5;  //cruciform
    return pow(y, 6) - (pow(a*x, 2) - pow(x, 6));  //butterfly curve
    //return pow(x, 6) + 3*(x, 4)*(y*y -3) + 3*x*x*y*y*(y*y + 2) + pow(y, 6) - pow(y, 4);  //cycloid of ceva, get pacman shape
    //return pow(x*x + y*y, 3) - 4*a*a*x*x*y*y; //four leaf clover 
  //return -52521875*pow(a, 12) - 1286250*pow(a, 10)*b - 32025*pow(a, 8)*b*b + 93312*pow(a, 7)*(pow(x, 5) - 10*x*x*x*y*y + 5*x*pow(y, 4)) - 812*pow(a, 6)*pow(b, 3) - 21*pow(a, 4)*pow(b, 4) - 42*a*a*pow(b, 5) + pow(b, 6); 
  
}

float density(Vector3f point) {
    int x = point[0]*(1/STEP_X) + GRID_X_MAX * (1/STEP_X);  
    int y = point[2]*(1/STEP_Z) + GRID_Z_MAX * (1/STEP_Z); 
    float dist = closeCurve(point[0], point[2]); 
    int r = rand() % 100; 
 
    float height = h.heights[x * HEIGHTMAP_SIZE + y]/150; 
    if(height < 0) height *= -1; 
    
    /*if(dist > 4 && (y % 3 == 0) ) height = 0;
    if(dist > 3 && rand() % 4 == 3 ) height = 0.02;
    if(dist > 5) height = 0;  
    
    if(dist > 3.5) height *= 0.3; 
    if (dist > 2) height *= 0.2; */
    
    if(dist > 2*GRID_X_MAX) height = 0; 
    else if (dist > (GRID_X_MAX - 1) ){
      if(r % 4 == 0) height *= 0.5;
      else height *= 0.2; 
    } 
    
    else if (dist  > (GRID_X_MAX - 5)) height *= 0.1; //height = -0.4;
    else if (dist > (GRID_X_MAX - 3)) height *= 0.5; //height = -0.1;
    else if (dist - (GRID_X_MAX - 0.7)> 0) height *= 0.35; 
    else if (dist - (GRID_X_MAX - 1.0)> 0) height *= 0.5; 
    else if (dist - (GRID_X_MAX - 3.0)> 0) height *= 0.7; 

    return point(1) - height;
    
    //return pow(1.0f-sqrt(pow(point(0), 2.0f) + pow(point(1), 2.0f)), 2.0f) + pow(point(2), 2.0f) - .5;
    //return tan(3.14*point(0))-point(1);
}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;

vector_tri tris;

Grid *grid;
float ustep, vstep, error, max_z = 0, focus = 71, minfocus = 3, maxfocus = 75; 
float rotUD = 15, rotLR = 0, rotQE = 0, ytrans = 0, xtrans = 0, ztrans = 0;
bool flat, wireframe, adaptive, dof, showrain, fog, takePic;
GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};
GLfloat mat_shininess[] = {128.0f};
GLfloat mat_ambient[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat mat_diffusion[] = {0.0f, 0.3f, 0.0f, 1.0f};

GLfloat light_position[] = {5.0f, 1.0f, 5.0f, 1.0f};
GLfloat light_diffuse[] = {.5f, 0.5f, 0.4f, 1.0f};
GLfloat light_specular[] = {0.1f, 0.1f, 0.1f, 0.0f};
GLfloat light_ambient[] = {0.07f, 0.02f, 0.05f, 1.0f};

bool mouseButtony_down_left = false, mouseButtonx_down = false, mouseButtony_down_right = false; 
int mouse_yClick_left = 0, mouse_xClick = 0,mouse_yClick_right = 0;  

GLuint vs, fs, sp; //shaders 
GLfloat waveTime = 0.0, waveWidth = 0.1, waveHeight = 3.0, waveFreq = 0.1; 
GLint waveTimeLoc, waveWidthLoc, waveHeightLoc; 

GLint fogMode; 
Vector3f* rain; 
int numdrops = 1000; 



void drawTris() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffusion);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    
    Dot sun = returnSunPos();
    light_position[0] = sun(0)*10;
    light_position[1] = sun(1)*10;
    light_position[2] = sun(2)*10;
    
    float sunRot = returnSunRot();

    float strength = 0.0f; // [0,1]
    
    if (sunRot < 3.14159) {
        // night
        strength = 1.0f; 
    } else if (sunRot < 3.14159 + 1.0) {
        // sunrise
        strength = 1.0f - (sunRot - pi);
    } else if (sunRot > (2*pi - 1.0)) {
        // sunset
        strength = 1.0f - (2*pi - sunRot);
    }
    
    // strength/color of sunlight;
    light_diffuse[0] = 0.5f + strength/2;
    light_diffuse[1] = 0.55f - strength/8;
    light_diffuse[2] = 0.55f - strength/4;



    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glEnable(GL_TEXTURE_2D);
    
    
	for (int i = 0; i < (int)tris.size(); i++) {
		tris[i].draw();
	}
    
    glDisable(GL_TEXTURE_2D);
}

//****************************************************
// GLUT and Initialization Functions
//****************************************************

void accFrustum(GLdouble left, GLdouble right, GLdouble bottom,
    GLdouble top, GLdouble near, GLdouble far, GLdouble pixdx, 
    GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, 
    GLdouble focus)
{
    GLdouble xwsize, ywsize; 
    GLdouble dx, dy;
    GLint viewport[4];

    glGetIntegerv (GL_VIEWPORT, viewport);

    xwsize = right - left;
    ywsize = top - bottom;
    dx = -(pixdx*xwsize/(GLdouble) viewport[2] + 
            eyedx*near/focus);
    dy = -(pixdy*ywsize/(GLdouble) viewport[3] + 
            eyedy*near/focus);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (left + dx, right + dx, bottom + dy, top + dy, 
        near, far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef (-eyedx, -eyedy, 0.0);
   
    
}

void accPerspective(GLdouble fovy, GLdouble aspect, 
    GLdouble near, GLdouble far, GLdouble pixdx, GLdouble pixdy, 
    GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
    GLdouble fov2,left,right,bottom,top;
    fov2 = ((fovy*M_PI) / 180.0) / 2.0;

    top = near / (cos(fov2) / sin(fov2));
    bottom = -top;
    right = top * aspect;
    left = -right;

    accFrustum (left, right, bottom, top, near, far,
        pixdx, pixdy, eyedx, eyedy, focus);
}

void drawRain(){
    glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    
    GLfloat rain_diffuse[] = {1.0f, 1.0f, 1.0f, 0.3f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, rain_diffuse);        
    glLightfv(GL_LIGHT0, GL_AMBIENT, rain_diffuse);
    glEnable(GL_LINE_SMOOTH);
    
    for(int i = 0; i < numdrops; i++){

    
        glBegin(GL_LINES); 
             glVertex3f(rain[i](0) * 0.01, rain[i](1) * 0.01, rain[i](2) *0.01);
             glVertex3f(rain[i](0) * 0.01, rain[i](1)*0.01 + 0.1, rain[i](2)*0.01);
        glEnd(); 
        rain[i](1) -= 10; 
        if(rain[i](1) < -250) rain[i](1) = 250; 

    }
   
   
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}



void initRain(){
    srand(time(NULL)); 
    for(int i = 0; i < numdrops; i ++){
        rain[i] = Vector3f((rand() % 500)-250, (rand() % 500) -250, (rand() % 500) -250); 
    }  
}






void reshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	
	initCamera();

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(focus, (GLfloat) w/ (GLfloat) h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void drawOceanShader(){
    glUseProgram(sp); 
    
    /* Change time */
    glUniform1f(waveTimeLoc, waveTime);
    glUniform1f(waveWidthLoc, waveWidth);
    glUniform1f(waveHeightLoc, waveHeight);
    
    
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    float subdiv = 1;  
    glBegin(GL_QUADS); 
    for(int i = -GRID_X_MAX; i < GRID_X_MAX; i++){
        for(int j = -GRID_Z_MAX; j < GRID_Z_MAX; j++){
                glVertex2f(i, j);
                glVertex2f(i + subdiv,  j); 
                glVertex2f(i + subdiv,j + subdiv);
                glVertex2f(i, j + subdiv);
        }
    }
    glEnd(); 
    waveTime += waveFreq; 
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(focus, (GLfloat) viewport.w/ (GLfloat) viewport.h, 0.5, 40.0);
        glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	
    glTranslatef(xtrans, ytrans, ztrans);    
	glRotatef(rotUD, 1, 0, 0);
	glRotatef(rotLR, 0, 1, 0);
	glRotatef(rotQE, 0, 0, 1);
    
    if (flat) {
		glShadeModel(GL_FLAT);
	} else {
		glShadeModel(GL_SMOOTH);
	}
	drawTris();
    drawSkyBox();
    
    if (showrain) drawRain();   
     
    drawSun(); // behind clouds?
    
    //draw reflection
       
    drawClouds();    
    glMatrixMode(GL_MODELVIEW);
    glScalef(1, -1, 1); 
    drawTris(); 
    drawOcean();
    //drawOceanShader(); 
    drawClouds(); // clouds have transparency, so draw last!

	glPopMatrix();
	glutSwapBuffers();

    if(takePic) {
        takePic = false;
        takePicture();
        writeImage("Sex_Piston.png");
        renderBlackandWhite();
        writeImage("Sex_Pistol_BW.png");
        takePicture();
        bayerFilter();
        writeImage("Sex_Pistol_Bayer.png");
    }
} 

void idle (void) {
	glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y) {
   // cout << "key:" << key << endl;
	switch(key) {
	case GLUT_KEY_RIGHT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			xtrans += .15f*(focus/60.0);
		} else {
			rotLR += 1.0f*(focus/60.0);
		}
		break;
	case GLUT_KEY_LEFT:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			xtrans -= .15f*(focus/60.0);
		} else {
			rotLR -= 1.0f*(focus/60.0);
		}
		break;
	case GLUT_KEY_UP:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			ytrans += .15f*(focus/60.0);
		} else {
			rotUD -= 1.0f*(focus/60.0); 
		}
		break;
	case GLUT_KEY_DOWN:
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			ytrans -= .15f*(focus/60.0);
		} else {
			rotUD += 1.0f*(focus/60.0);
		}
		break;
	}
}

void mouseButton(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        mouseButtony_down_left = (state == GLUT_DOWN)? true:false; 
        mouse_yClick_left = y; 
    }
    if(button == GLUT_RIGHT_BUTTON){
        mouseButtonx_down = (state == GLUT_DOWN)? true:false; 
        mouseButtony_down_right = (state == GLUT_DOWN)? true:false;
        mouse_yClick_right = y; 
        mouse_xClick = x; 
    }
}
void mouseMotion(int x, int y){
    if(mouseButtony_down_left){
        if(y < mouse_yClick_left){
            focus *= pow(0.95,  0.001*abs((int)(y - mouse_yClick_left))); 
            if(focus < minfocus) focus = minfocus; 
        }
        else{
            focus *= pow(1.0f/0.95f,  0.001*abs((int)(y - mouse_yClick_left))); 
           if (focus > maxfocus) focus = maxfocus; 
            
        }
    }
    if(mouseButtonx_down){
        if(x < mouse_xClick){//right
            rotLR -= 0.001 * abs((int)(x - mouse_xClick))*( 1.0f*(focus/60.0));
        }
        else{//left
            rotLR += 0.001 * abs((int)(x - mouse_xClick)) *(1.0f*(focus/60.0));
        }
    }
    if(mouseButtony_down_right){
        if(y < mouse_yClick_right){
            rotUD -=  0.001 * abs((int)(y - mouse_yClick_right))*1.0f*(focus/60.0); 
            if(rotUD < 6) rotUD = 6; 
        }
        else{
            rotUD +=  0.001 * abs((int)(y - mouse_yClick_right))*1.0f*(focus/60.0);
            if(rotUD > 26) rotUD = 26; 
        }
        
    }
    glutPostRedisplay(); 
}
void debug(){
    cout<<"The rotUD is"<<rotUD<<endl; 
    cout<<"The focus is"<<focus<<endl; 
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {  
    case 'D': debug(); break;
    case 'P':
        takePic = true;
        break;
    case 'F':
        fog = !fog;
        if (fog)
            glEnable(GL_FOG);
        else
            glDisable(GL_FOG);
            {
            GLfloat fogColor[4] = {0.6, 0.6, 0.6, 1.0};
            fogMode = GL_EXP;
            glFogi (GL_FOG_MODE, fogMode);
            glFogfv (GL_FOG_COLOR, fogColor);
            glFogf (GL_FOG_DENSITY, 0.10);
            glHint (GL_FOG_HINT, GL_NICEST);
            glFogf (GL_FOG_START, 1.0);
            glFogf (GL_FOG_END, 5.0);
            }
            break;
    case 'f':
         if (fogMode == GL_EXP) {
            fogMode = GL_EXP2;
            printf ("Fog mode is GL_EXP2\n");
         }
         else if (fogMode == GL_EXP2) {
            fogMode = GL_LINEAR;
            printf ("Fog mode is GL_LINEAR\n");
         }
         else if (fogMode == GL_LINEAR) {
            fogMode = GL_EXP;
            printf ("Fog mode is GL_EXP\n");
         }
         glFogi (GL_FOG_MODE, fogMode);
         glutPostRedisplay();
         break;
    case 'r':
        showrain = !showrain; 
        break; 
    case 's':
        flat = !flat;
        break;
    case 'z':
        ztrans += .15f*(focus/60.0);
        break;
    case 'c':
        ztrans -= .15f*(focus/60.0);
        break;
    case 'i':
        ytrans -= .15f*(focus/60.0);
        break;
    case 'k':
        ytrans += .15f*(focus/60.0);
        break;
    case 'w':
        wireframe = !wireframe;
        break;
    case '+':
        focus *= 0.95f;
        if (focus < 0.1f) {
                focus = 0.1f;
        }
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(focus, (GLfloat) viewport.w/ (GLfloat) viewport.h, 1.0, 40.0);
        glMatrixMode(GL_MODELVIEW);
        break;
    case '-':
        focus *= 1.0f/0.95f;
        if (focus > 89.1f) {
                focus = 89.99f;
        }
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(focus, (GLfloat) viewport.w/ (GLfloat) viewport.h, 1.0, 40.0);
        glMatrixMode(GL_MODELVIEW);
            break;
    case 'q':
        rotQE += 1.0f*(focus/60.0);
        break;
    case 'e':
        rotQE -= 1.0f*(focus/60.0);
        break;
    case '1':
        updateSunRot(returnSunRot() - 0.55);
        break;
    case '2':
        updateSunRot(returnSunRot() - 0.15);
        break;
    case '3':
        updateSunRot(returnSunRot() + 0.15);
        break;
    case '4':
        updateSunRot(returnSunRot() + 0.55);
        break;
    case '5':
        cout << "sunRot: " << returnSunRot() << endl;
        break;
    case 27:
        exit(0);
        break;
    }
}

void loadData(string file) {
	ifstream inpfile(file.c_str());
	if (!inpfile.is_open()) {
		cout << "Unable to open file" << endl;
		exit(0);
	} else {
		inpfile.close();
	}
}

void test(){
    for(int i = 0; i < h.size; i++){
        for(int j = 0; j < h.size; j++){
            printf("%f \n ", h.heights[i*h.size + j]); 
            
        }
      
    }
}


int main(int argc, char* argv[]) {
    
  cout << "start" << endl;
	viewport.h = 800;
	viewport.w = 800;

	clock_t t = clock();
	initCamera();
    initializeDensityFunction();

    rain = new Vector3f[numdrops]; 

	grid = new Grid(Vector3f(0, 0, 0), stepsize, gridMax);

	grid->addTriangles(&tris, density);

	wireframe = false;
    showrain = false;

	t = clock() - t;

	cout << "Time taken in seconds: " << (float(t))/CLOCKS_PER_SEC << endl;
	fflush(stdout);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Final Project");

    initClouds();
    initSun();
    initSkyBox(); 
    initRain(); 
    initOcean();
    initTerrainTextures();
    cout << "Done with initialization " << endl;
    
	glClearColor(0, 0, 0, 0);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)viewport.w/(float)viewport.h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE); 

	glShadeModel(GL_FLAT);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
    glutMouseFunc (mouseButton);
    glutMotionFunc (mouseMotion);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}




