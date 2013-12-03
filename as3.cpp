#include "glUtil.h"
#include "terrain.h"
#include "textures/lodepng.h"

//****************************************************
// Some Classes and Global Standalone Functions
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
    //for(int i = 0; i < 10; i++) h.erode(16); 
    //h.smoothen(); 
}

float density(Vector3f point) {
    int x = point[0]*10 + 40;  
    int y = point[2]*10 + 40; 
    float height = h.heights[x * 100 + y]/200; 
    
    //int x = (int)floorf((point[0] + 4)/8 * 64);
    //int z = (int)floorf((point[2] + 4)/8 * 64);
    //float height = (h.heights[x * 64 + z])/200 * 3;  
    return point(1) - height;
    //return pow(1.0f-sqrt(pow(point(0), 2.0f) + pow(point(1), 2.0f)), 2.0f) + pow(point(2), 2.0f) - .5;
    //return tan(3.14*point(0))-point(1);
}

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
Vector3f gridMax = Vector3f(4, 4, 4); 
Vector3f stepsize = Vector3f(.1, .1, .1); 
vector_tri tris;

Grid *grid;
float ustep, vstep, error, max_z = 0, focus = 60;
float rotUD = 0, rotLR = 0, rotQE = 0, ytrans = 0, xtrans = 0, ztrans = 0;
bool flat, wireframe, adaptive;
GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};
GLfloat mat_shininess[] = {128.0f};
GLfloat mat_ambient[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat mat_diffusion[] = {0.0f, 0.3f, 0.0f, 1.0f};
GLfloat light_position[] = {5.0f, 1.0f, 5.0f, 1.0f};
GLfloat light_diffuse[] = {.3f, 0.5f, 0.2f, 1.0f};
GLfloat light_specular[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

void drawTris() {
	for (int i = 0; i < (int)tris.size(); i++) {
		tris[i].draw();
	}
}

//****************************************************
// GLUT and Initialization Functions
//****************************************************


GLuint AddTextureToOpenGL(unsigned w, unsigned h, void * ptr) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 1 ? GL_REPEAT : GL_CLAMP );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 1 ? GL_REPEAT : GL_CLAMP );
    
 
    //void * ptr = &image[0];
    gluBuild2DMipmaps( GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
    
    return texture;
}


GLuint LoadTextureFromPNG(const char * filename) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; 
    lodepng::State state; //optionally customize this one
    unsigned int w, h;

    lodepng::load_file(png, filename); 
    
    unsigned error = lodepng::decode(image,  w,  h, state, png); // decode file into image
    if(error) std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;


    //void * ptr = &image[0];
    return AddTextureToOpenGL(w, h, &image[0]);

}



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
            if (indx == 1) {
                cout << "pn: " << pn << endl;
            }
            
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


GLuint t_SkyBoxTop,t_SkyBoxBottom;
GLuint t_Clouds1, t_Clouds2;





float skyboxMax = 20.0;
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

class Dot{ 
public:
    Dot(double, double, double); 
    double x, y, z;
    Dot operator - (Dot);  
};
Dot::Dot(double nx, double ny, double nz) {
    x = nx; y = ny; z = nz;
}
Dot Dot::operator-(Dot d) {
    return Dot(x - d.x, y - d.y, z - d.z);
}

void drawSkyBoxDot(Dot d, double u, double v) {
    glTexCoord2f(u, v); glVertex3f(d.x, d.y, d.z);
}

//Dot sky_bur, sky_bul, sky_bdr, sky_bdl
//Dot sky_fur, sky_ful, sky_fdr, sky_fdl

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
    
     //glDepthMask(GL_FALSE);
     //glEnable(GL_BLEND);
     //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
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

    drawSkyBox();
	drawTris();
    drawClouds(); // clouds have transparency, so draw last!
    
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

	clock_t t = clock();

    initializeDensityFunction(); 

	grid = new Grid(Vector3f(0, 0, 0), stepsize, gridMax);

	grid->addTriangles(&tris, density);

	wireframe = false;

	t = clock() - t;

	cout << "Time taken in seconds: " << (float(t))/CLOCKS_PER_SEC << endl;
	fflush(stdout);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Final Project");
    
    initializeSkyBox(); // has to be done after glutinit
	
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




