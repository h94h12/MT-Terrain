#include <cstdlib>

#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include "primitives.h"
#include "terrain.h"



using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;

vector<Triangle> tris; 
bool flatshading = 1; 
bool uniformFlag = 1;
bool wireframe = 0; 
float rotate_x = 0; 
float rotate_y = 0; 
float rotate_z = 0; 

float trans_x = 0; 
float trans_y = 0; 
float trans_z = -10; 

float cubesize = 0.1; 

float epsilon = 0.0000001; 
int limit = 15; //limit iterations for bisection

//Vertex Normals
Vector TOP = Vector(0, 1, 0); 
Vector BOTTOM = Vector(0, -1, 0); 
Vector RIGHT = Vector(1, 0, 0); 
Vector LEFT = Vector(-1, 0, 0); 
Vector FRONT = Vector(0, 0, 1); 
Vector BACK = Vector(0, 0, -1); 
Vector DIAG_BACK = Vector(1, 0, -1); 
Vector DIAG_FRONT = Vector(-1, 0, 1); 

HeightMap h; 

//****************************************************
// Implicit Functions
//****************************************************
bool function(float x, float y, float z){
   int x1 = x*10 + 40;  
   int y1 = z*10 + 40; 
   float height = h.heights[x1 * 100 + y1]/(float)200;
   return (y - height) >= 0; 
   //return x*x + y*y + z*z - 1 >= 0; 
}
bool function(float x, float y, float z, float e){
   int x1 = x*10 + 40;  
   int y1 = z*10 + 40; 
   float height = h.heights[x1 * 100 + y1]/(float)200;
   return (y - height) >= e; 
   //return x*x + y*y + z*z >= e; 
}
bool function(Point p){
    return function(p.x, p.y, p.z); 
}
bool closeEnough(Point p){
    return function(p.x, p.y, p.z, epsilon); 
}

//****************************************************
// Lookup Tables
//****************************************************
map<Edge, bool> cube_corners;
map<Edge, float> cutpoints; 


bool checkTet(bool a, bool b, bool c, bool d){
    if(!(a ^ b) && !(b ^ c) && !(c ^ d) && !(a^d))
        return false; 
    return true; 
    
}    
//given that start and end have different signs, find intersection with isosurface
Point bisection(Point start, Point end, int iteration){
    Point mid = (start + end) * 0.5; 
    if ((iteration >= limit) || closeEnough(mid))
        return mid; 
    if(function(mid) != function(start))
        return bisection(start, mid, iteration + 1);
    else if(function(mid) != function(end))
        return bisection(mid, end, iteration + 1);
}

void getTri(Vertex v0, Vertex v1, Vertex v2, Vertex v3){
    vector<Point> tripoints; 
    if(v0.sign != v1.sign){
        tripoints.push_back(bisection(v0.point, v1.point, 0));
    }
    if(v0.sign != v2.sign){
        tripoints.push_back(bisection(v0.point, v2.point, 0));
    }
    if(v0.sign != v3.sign){
        tripoints.push_back(bisection(v0.point, v3.point, 0));
    }
    if(v1.sign != v2.sign){
        tripoints.push_back(bisection(v1.point, v2.point, 0));
    }
    if(v1.sign != v3.sign){
        tripoints.push_back(bisection(v1.point, v3.point, 0));
    }
    if(v2.sign != v3.sign){
        tripoints.push_back(bisection(v2.point, v3.point, 0));
    }
    
    if(tripoints.size() >= 3){
        tris.push_back(Triangle(tripoints[0], tripoints[1], tripoints[2])); 
    if(tripoints.size() == 4)
        tris.push_back(Triangle(tripoints[0], tripoints[2], tripoints[3])); 
            
    }
 //TO DO: GET NORMALS
}

void doTets(Cube* c){
    
    bool flt = c->FLT.sign; 
    bool nrt = c->NRT.sign;
    bool flb = c->FLB.sign; 
    bool nrb = c->NRB.sign;
    bool nlb = c->NLB.sign;
    bool frt = c->FRT.sign;
    bool frb = c->FRB.sign; 
    bool nlt = c->NLT.sign; 

    if(checkTet(flt, nrt, flb, nlt))
        getTri(c->FLT, c->NRT, c->FLB, c->NLT); 
    if (checkTet(nlt, nrt, nrb, flb))
        getTri(c->NLT, c->NRT, c->NRB, c->FLB); 
    if(checkTet(nlt, nrb, nlb, flb))
        getTri(c->NLT, c->NRB, c->NRB, c->FLB); 
    if(checkTet(flt, frt, frb, nrt))
        getTri(c->FLT, c->FRT, c->FRB, c->NRT); 
    if(checkTet(flb, nrt, nrb, frb))
        getTri(c->FLB, c->NRT, c->NRB, c->FRB);
    if(checkTet(flt, flb, nrt, frb))
        getTri(c->FLT, c->FLB, c->NRT, c->FRB); 
}

void initializeLookupTable(){
    
}


void polygonize(){
    //initializeLookupTable(); 
    
    for(float z = -4; z <= 4 - cubesize; z += 0.05){
        for(float y = -4; y <= 4 - cubesize; y += 0.05){
            for(float x = -4; x <= 4 - cubesize; x += 0.05){
                
                //TO DO:
                //update hashtable
                Cube c; 
                
                //sample function at vertices
                
                c.FLB = Vertex(Point(x, y, z), function(x, y, z)); 
                c.FLT = Vertex(Point(x, y+cubesize, z), function(x, y+cubesize, z)); 
                c.FRB = Vertex(Point(x+cubesize, y, z), function(x+cubesize, y, z)); 
                c.FRT = Vertex(Point(x+cubesize, y+cubesize, z), function(x+cubesize, y+cubesize, z)); 
                c.NLB = Vertex(Point(x, y, z+cubesize), function(x, y, z+cubesize));
                c.NLT = Vertex(Point(x, y+cubesize, z+cubesize), function(x, y+cubesize, z+cubesize)); 
                c.NRB = Vertex(Point(x+cubesize, y, z+cubesize), function(x+cubesize, y, z+cubesize)); 
                c.NRT = Vertex(Point(x+cubesize, y+cubesize, z+cubesize), function(x+cubesize, y+cubesize, z+cubesize)); 
                
                //get tetrahedrons
                doTets(&c); 
        
            
            
            
            }
        }
    }
    
    
}

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}

void myKeyboardFunc(unsigned char key, int x, int y){
	//toggle between smooth and flat shading
    if(key == 's') flatshading = !flatshading; 
    //toggle between filled and wireframe mode
    if(key == 'w') wireframe = !wireframe; 
    //zoom in and out

    if (key == '+') trans_z += .1;
    if (key == '-') trans_z -= .1;
            
}

void mySpecialFunc(int key, int x, int y){

       int mod = glutGetModifiers(); 
       
        if (key == GLUT_KEY_UP)
            if (mod == GLUT_ACTIVE_SHIFT) trans_y += .1;
            else rotate_x += 2;
        if (key == GLUT_KEY_DOWN)
            if (mod == GLUT_ACTIVE_SHIFT) trans_y -= .1;
            else rotate_x -= 2;
        if (key == GLUT_KEY_RIGHT)
            if (mod == GLUT_ACTIVE_SHIFT) trans_x += .1;
            else rotate_z -= 2;
        if (key == GLUT_KEY_LEFT)
            if (mod == GLUT_ACTIVE_SHIFT) trans_x -= .1;
            else rotate_z += 2;
                 
        if (key == GLUT_KEY_PAGE_UP) rotate_y -= 2;
        if (key == GLUT_KEY_PAGE_DOWN) rotate_y += 2;
                 

    if (abs(rotate_y) > 360) rotate_y = 0;
    if (abs(rotate_x) > 360 ) rotate_x = 0; 
    
    
}

void initScene(){

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent

  myReshape(viewport.w,viewport.h);
	
}





void draw() {
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < tris.size(); i++){
        glVertex3f(tris[i].p0.x, tris[i].p0.y, tris[i].p0.z); 
        glVertex3f(tris[i].p1.x, tris[i].p1.y, tris[i].p1.z); 
        glVertex3f(tris[i].p2.x, tris[i].p2.y, tris[i].p2.z); 
    }
    
    
    glEnd(); 
    
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer
  
  glLoadIdentity();				        // make sure transformation is "zero'd"
  
  gluPerspective(30.0,800.0/800.0,0.1,500.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);
  
  
  static float alpha = 0; 

  glTranslatef(trans_x, trans_y, trans_z-5); 
  
  glRotatef(rotate_x, 1, 0, 0);
  glRotatef(rotate_y, 0, 1, 0);
  glRotatef(rotate_z, 0, 0, 1);
  
    
  if(flatshading) glShadeModel(GL_FLAT);
  else glShadeModel(GL_SMOOTH); 

  if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
  
  draw(); 
 
  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
  glutPostRedisplay();
}
void myFrameMove() {
  //nothing here for now
#ifdef _WIN32
  Sleep(10);                                   //give ~10ms back to OS (so as not to waste the CPU)
#endif
  glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

 


//****************************************************
// main and parser for terminal arguments
//****************************************************
int main(int argc, char *argv[]) {
	
        h = HeightMap(100); 
        h.addPerlinNoise(10); 
	    polygonize(); 
    
    
		//This initializes glut
		glutInit(&argc, argv);

		//This tells glut to use a double-buffered window with red, green, and blue channels 
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        
        
		// Initalize theviewport size
		viewport.w = 800;
		viewport.h = 800;
		

		//The size and position of the window
		glutInitWindowSize(viewport.w, viewport.h);
		glutInitWindowPosition(0,0);
		glutCreateWindow(argv[0]);
        
        
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 50.0 };
        GLfloat light_position[] = { 2.0, 2.0, 0.0, 0.0 };
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glShadeModel (GL_SMOOTH);

        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            
        glClearColor(0, 0, 0, 0); 
        glEnable(GL_DEPTH_TEST);  //IMPORTANT
        glEnable(GL_COLOR_MATERIAL); 
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

     
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        glFrustum (-1.0, 1.0, -1.0, 1.0, -4.5, 1.0);
        glMatrixMode (GL_MODELVIEW);

        
		glutDisplayFunc(myDisplay);				// function to run when its time to draw something
		glutReshapeFunc(myReshape);				// function to run when the window gets resized
		//glutIdleFunc(myFrameMove);  			// this makes it animated
		glutKeyboardFunc(myKeyboardFunc);
		glutSpecialFunc(mySpecialFunc); //need this for UP/DOWN/LEFT/RIGHT
		
		
		
		glutMainLoop();							// infinite loop that will keep drawing and resizing

		
		

	

    return 0;
}
