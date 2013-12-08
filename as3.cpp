#include "glUtil.h"
#include "terrain.h"
#include "Sky.h"

//****************************************************
// Some Classes and Global Standalone Functions
//****************************************************

#define GRID_X_MAX 5
#define GRID_Y_MAX 5
#define GRID_Z_MAX 5

#define STEP_X 0.1
#define STEP_Y 0.1
#define STEP_Z 0.1

#define HEIGHTMAP_SIZE 100

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

HeightMap h;

GLuint grassTexture;

Vector3f gridMax = Vector3f(GRID_X_MAX, GRID_Y_MAX, GRID_Z_MAX); 
Vector3f stepsize = Vector3f(STEP_X, STEP_Y, STEP_Z); 

void initializeDensityFunction(){
    h = HeightMap(HEIGHTMAP_SIZE, rand()); 
    h.addPerlinNoise(16); 
    for(int i = 0; i < 10; i++) h.erode(16); 
    h.smoothen(); 
}

float density(Vector3f point) {
    int x = point[0]*(1/STEP_X) + GRID_X_MAX * (1/STEP_X);  
    int y = point[2]*(1/STEP_Z) + GRID_Z_MAX * (1/STEP_Z); 
    float dist = point[0]*point[0] + point[2]*point[2];
 
    float height = h.heights[x * HEIGHTMAP_SIZE + y]/200; 
    if (height < 0) height *= -1; 
    if (dist - (GRID_X_MAX - 0.1) > 0) height = -1; 
    else if (dist - (GRID_X_MAX - 0.2) > 0) height = -0.4;
    else if (dist - (GRID_X_MAX - 0.4) > 0) height = -0.1;
    else if (dist - (GRID_X_MAX - 0.7)> 0) height *= 0.1; 
    else if (dist - (GRID_X_MAX - 1.0)> 0) height *= 0.5; 
 
    return point(1) - height;
    
    //return pow(1.0f-sqrt(pow(point(0), 2.0f) + pow(point(1), 2.0f)), 2.0f) + pow(point(2), 2.0f) - .5;
    //return tan(3.14*point(0))-point(1);
}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;

//Vector3f gridMax = Vector3f(4, 4, 4); 
//Vector3f stepsize = Vector3f(.1, .1, .1); 
//Vector3f stepsize = Vector3f(.05, .05, .05); 

vector_tri tris;

Grid *grid;
float ustep, vstep, error, max_z = 0, focus = 60;
float rotUD = 0, rotLR = 0, rotQE = 0, ytrans = 0, xtrans = 0, ztrans = 0;
bool flat, wireframe, adaptive, dof, showrain, fog;
GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};
GLfloat mat_shininess[] = {128.0f};
GLfloat mat_ambient[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat mat_diffusion[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat light_position[] = {5.0f, 1.0f, 5.0f, 1.0f};
GLfloat light_diffuse[] = {.5f, 0.5f, 0.4f, 1.0f};
GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat light_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};

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
    //cout << sun(0) << endl;
    //light_position[2] = -10.0f;

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    
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
    for(int i = 0; i < numdrops; i++){
        GLfloat rain_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, rain_diffuse);        
        glLightfv(GL_LIGHT0, GL_AMBIENT, rain_diffuse);
    
        glBegin(GL_LINES); 
            glVertex3f(rain[i](0) * 0.01, rain[i](1) * 0.01, rain[i](2) *0.01);
             glVertex3f(rain[i](0) * 0.01, rain[i](1)*0.01 + 0.1, rain[i](2)*0.01);
        glEnd(); 
        rain[i](1) -= 10; 
        if(rain[i](1) < -250) rain[i](1) = 250; 

    }
   
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

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(focus, (GLfloat) w/ (GLfloat) h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//if (flat) {
	//	glShadeModel(GL_FLAT);
	//} else {
	//	glShadeModel(GL_SMOOTH);
	//}

	glPushMatrix();
	
	glRotatef(rotUD, 1, 0, 0);
	glRotatef(rotLR, 0, 1, 0);
	glRotatef(rotQE, 0, 0, 1);
    
    glTranslatef(xtrans, ytrans, ztrans);

    drawSkyBox();
	drawTris();
    if (showrain) drawRain();    
    drawSun(); // behind clouds?
    drawOcean();
    drawClouds(); // clouds have transparency, so draw last!
    
    //need to draw reflection
    /*glMatrixMode(GL_MODELVIEW);
    glRotatef(180, 1, 0, 0); 
    glTranslatef(0, -2, 0); 
    drawTris();*/ 

	glPopMatrix();
	glutSwapBuffers();
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
         //cout << "down!" << endl;
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
            //cout << "down shift!" << endl;
			ytrans -= .15f*(focus/60.0);
		} else {
			rotUD += 1.0f*(focus/60.0);
		}
		break;
	}
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {   
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
            glFogf (GL_FOG_DENSITY, 0.25);
            glHint (GL_FOG_HINT, GL_DONT_CARE);
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
    case 'n': // temp
        ytrans -= .15f*(focus/60.0);
        break;
    case 'm': // temp
        ytrans += .15f*(focus/60.0);
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
    //initGrassTexture();
    grassTexture = LoadTextureFromPNG("textures/grass.png");
    
    //grassTexture = LoadTextureFromPNG("textures/sm64_ocean.png");
    
	
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
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}




