#include "glUtil.h"
#include "terrain.h"

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};

HeightMap h; 

void initializeDensityFunction(){
    h = HeightMap(100); 
    h.addPerlinNoise(10); 
    //for(int i = 0; i < 10; i++) h.erode(16); 
    //h.smoothen(); 
  
}

float density(Vector3f point) {
   int x = point[0]*10 + 40;  
   int y = point[2]*10 + 40; 
   float height = h.heights[x * 100 + y]/(float)200;
   return point[1] - height; 
   
    //return point[0]*point[0] +  point[1]*point[1] + point[2]*point[2] - 0.3; 
   
   //return pow(1 - sqrt(point[0]*point[0] + point[1]*point[1]), 2) + point[2]*point[2] - 0.2; 
   

}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Vector3f gridMax = Vector3f(4, 4, 8); 
Vector3f stepsize = Vector3f(0.1, 0.2, 0.1); 

Grid *grid;
float ustep, vstep, error, max_z = 0, focus = 45;
float rotUD = 0, rotLR = 0, rotQE = 0, ytrans = 0, xtrans = 0, ztrans = 0;
bool flat, wireframe, adaptive, drawTets, dof; 
GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};
GLfloat mat_shininess[] = {128.0f};
GLfloat mat_ambient[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat mat_diffusion[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat light_position[] = {5.0f, 1.0f, 5.0f, 1.0f};
GLfloat light_diffuse[] = {.3f, 0.5f, 0.2f, 1.0f};
GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

GLint fogMode; 



//****************************************************
// GLUT and Initialization Functions
//****************************************************

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

void display(void) {
    if(dof){
        int jitter; 
        GLint vp[4]; 
        /*float j8[16] = {-0.334818,  0.435331, 
                        0.286438, -0.393495,
                        0.459462,  0.141540,
                        -0.414498, -0.192829,
                        -0.183790,  0.082102,
                        -0.079263, -0.317383,
                        0.102254,  0.299133,
                        0.164216, -0.054399}; */
        float j8[6] = {-0.373411, -0.250550,
                        0.256263,  0.368119,
                        0.117148, -0.117570}; 
        
        
        
        glGetIntegerv(GL_VIEWPORT, vp); 
        glClear(GL_ACCUM_BUFFER_BIT);
        
        for(jitter = 0; jitter < 3; jitter++){
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
             accPerspective (45.0, 
             (GLdouble) vp[2]/(GLdouble) vp[3], 
             0.1, 50.0, 0.0, 0.0,
             0.03*j8[jitter * 2], 0.03*j8[jitter * 2 + 1], 2.0);
            
        //do usual rendering
        
            gluLookAt(0, 0, 3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        
        glPushMatrix();
        glTranslatef(xtrans, ytrans, ztrans);
        glRotatef(rotUD, 1, 0, 0);
        glRotatef(rotLR, 0, 1, 0);
        glRotatef(rotQE, 0, 0, 1);
        grid->draw();
        glPopMatrix();
         glAccum(GL_ACCUM, 0.33); 
        }
            glAccum(GL_RETURN, 1.0); 
        glFlush(); 
        
    }
    else{
    
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glTranslatef(xtrans, ytrans, ztrans);
        glRotatef(rotUD, 1, 0, 0);
        glRotatef(rotLR, 0, 1, 0);
        glRotatef(rotQE, 0, 0, 1);
        grid->draw();
        glPopMatrix();

    }
    glutSwapBuffers(); 

}

void idle (void) {
	glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y) {
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

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
    case '1':
       dof = !dof; break;     
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
    
	case 's':
		if (flat) {
			flat = false;
		} else {
			flat = true;
		}
		break;
	case 'z':
		ztrans += .15f*(focus/60.0);
		break;
	case 'c':
		ztrans -= .15f*(focus/60.0);
		break;
	case 'p':
		if (drawTets) {
			drawTets = false;
		} else {
			drawTets = true;
		}
		break;
	case 'w':
		if (wireframe) {
			wireframe = false;
		} else {
			wireframe = true;
		}
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

void initialize(){
    viewport.h = 800;
	viewport.w = 800;
    
    initializeDensityFunction(); 
	grid = new Grid(Vector3f(0, 0, 0), stepsize, gridMax, density);

	wireframe = false;
	drawTets = false;
    dof = false; 


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE); 
    
    glEnable(GL_FOG);
   {
      GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

      fogMode = GL_EXP;
      glFogi (GL_FOG_MODE, fogMode);
      glFogfv (GL_FOG_COLOR, fogColor);
      glFogf (GL_FOG_DENSITY, 0.35);
      glHint (GL_FOG_HINT, GL_DONT_CARE);
      glFogf (GL_FOG_START, 1.0);
      glFogf (GL_FOG_END, 5.0);
   }
   glClearColor(0.5, 0.5, 0.5, 1.0);  /* fog color */

	glShadeModel(GL_FLAT);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffusion);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)viewport.w/(float)viewport.h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    

}

int main(int argc, char* argv[]) {

    initialize(); 

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ACCUM);
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Final Project");
	initialize(); 

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}




