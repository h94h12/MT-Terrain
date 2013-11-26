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
    h.addPerlinNoise(20); 
    for(int i = 0; i < 10; i++) h.erode(16); 
    h.smoothen(); 
  
}

float density(Vector3f point) {
    int x = point[0]*10 + 40;  
    int y = point[2]*10 + 40; 
    float height = h.heights[x * 100 + y]/200; 
    
    
    //int x = (int)floorf((point[0] + 4)/8 * 64);
    //int z = (int)floorf((point[2] + 4)/8 * 64);
    //float height = (h.heights[x * 64 + z])/200 * 3;  
    return height - point[1]; 
}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Vector3f gridMax = Vector3f(4, 4, 4); 
Vector3f stepsize = Vector3f(.1, .1, .1); 

Grid *grid;
float ustep, vstep, error, max_z = 0, focus = 60;
float rotUD = 0, rotLR = 0, rotQE = 0, ytrans = 0, xtrans = 0, ztrans = 0;
bool flat, wireframe, adaptive, drawTets;
GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};
GLfloat mat_shininess[] = {128.0f};
GLfloat mat_ambient[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat mat_diffusion[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat light_position[] = {5.0f, 1.0f, 5.0f, 1.0f};
GLfloat light_diffuse[] = {.3f, 0.5f, 0.2f, 1.0f};
GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

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

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//if (flat) {
	//	glShadeModel(GL_FLAT);
	//} else {
	//	glShadeModel(GL_SMOOTH);
	//}

	glPushMatrix();
	glTranslatef(xtrans, ytrans, ztrans);
	glRotatef(rotUD, 1, 0, 0);
	glRotatef(rotLR, 0, 1, 0);
	glRotatef(rotQE, 0, 0, 1);
	grid->draw();
	glPopMatrix();
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

int main(int argc, char* argv[]) {
	viewport.h = 800;
	viewport.w = 800;
    

    

    initializeDensityFunction(); 
    //test(); 
	grid = new Grid(Vector3f(0, 0, 0), stepsize, gridMax, density);

	wireframe = false;
	drawTets = false;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Final Project");
	
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
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffusion);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}




