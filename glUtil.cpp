#include "glUtil.h"


Triangle::Triangle(Vector3f v1, Vector3f v2, Vector3f v3) {
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
}


GLuint rockTexture; // locally used only
GLuint grassTexture;
GLuint sandTexture;
GLuint sandrock1Texture; // fades from sand to rock
GLuint sandrock2Texture;
GLuint sandrock3Texture;

void initTerrainTextures() {
    grassTexture = LoadTextureFromPNG("textures/grass.png");
    rockTexture = LoadTextureFromPNG("textures/rock.png");
    sandTexture = LoadTextureFromPNG("textures/sand.png");
    sandrock1Texture = LoadTextureFromPNG("textures/sandrock1.png");
    sandrock2Texture = LoadTextureFromPNG("textures/sandrock2.png");
    sandrock3Texture = LoadTextureFromPNG("textures/sandrock3.png");
    
    
}

void Triangle::draw() {
    if (wireframe) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(v1(0), v1(1), v1(2));
        glVertex3f(v2(0), v2(1), v2(2));
        glVertex3f(v3(0), v3(1), v3(2));
        glEnd();
    } else {
        // Texturing:
        double height = abs(v1(1)) + abs(v2(1)) + abs(v3(1));
        double mmin = min(min(v1(1), v2(1)), v3(1));
        double mmax = max(max(v1(1), v2(1)), v3(1));
        double dist = mmax - mmin;
        
        if (height < 0.005) return; 
        if (height < 0.05) {
            glBindTexture(GL_TEXTURE_2D, sandTexture);
        } else if (height < 0.11) {
            glBindTexture(GL_TEXTURE_2D, sandrock1Texture);
        } else if (height < 0.17) {
            glBindTexture(GL_TEXTURE_2D, sandrock2Texture);
        } else if (height < 0.25) {
            glBindTexture(GL_TEXTURE_2D, sandrock3Texture);
        } else {
            glBindTexture(GL_TEXTURE_2D, rockTexture);
        }
        
        
        
        glBegin(GL_TRIANGLES);
        Vector3f normal = (v1-v2).cross(v1-v3);
        if (normal.dot(Vector3f(0, 1, 0)) < 0) {
            normal *= -1;
        }

        
        
        glNormal3f(normal(0), normal(1), normal(2));
        glTexCoord2f(v1(0), v1(2)); glVertex3f(v1(0), v1(1), v1(2));
        glTexCoord2f(v2(0), v2(2)); glVertex3f(v2(0), v2(1), v2(2));
        glTexCoord2f(v3(0), v3(2)); glVertex3f(v3(0), v3(1), v3(2));
        
        //glTexCoord2f(v1(0)*0.125 + 0, v1(2)*0.125 + 0); glVertex3f(v1(0), v1(1), v1(2));
        //glTexCoord2f(v2(0)*0.125 + 0, v2(2)*0.125 + 0); glVertex3f(v2(0), v2(1), v2(2));
        //glTexCoord2f(v3(0)*0.125 + 0, v3(2)*0.125 + 0); glVertex3f(v3(0), v3(1), v3(2));
        glEnd();
    }
}

Tetrahedron::Tetrahedron(Vector3f *v1, Vector3f *v2, Vector3f *v3, Vector3f *v4) {
    this->v1 = v1; this->v2 = v2; this->v3 = v3; this->v4 = v4;
}

//Polygonization
void Tetrahedron::addTriangles(vector_tri *triangles, float (*func)(Vector3f)) {
    float v1v = func(*v1);
    float v2v = func(*v2);
    float v3v = func(*v3);
    float v4v = func(*v4);
    bool v1p = v1v > 0;
    bool v2p = v2v > 0;
    bool v3p = v3v > 0;
    bool v4p = v4v > 0;

    Vector3f cp1, cp2, cp3, cp4;

    if ((v1p&&v2p&&v3p&&v4p)||(!v1p&&!v2p&&!v3p&&!v4p)) {
        return;
    } else if ((!v1p&&v2p&&v3p&&v4p)||(v1p&&!v2p&&!v3p&&!v4p)) {
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp3 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((v1p&&!v2p&&v3p&&v4p)||(!v1p&&v2p&&!v3p&&!v4p)) {
        cp1 = *v2+(*v1-*v2)*(-v2v/(v1v-v2v));
        cp2 = *v2+(*v3-*v2)*(-v2v/(v3v-v2v));
        cp3 = *v2+(*v4-*v2)*(-v2v/(v4v-v2v));
        triangles->push_back(Triangle(cp1, cp2, cp3));       
    } else if ((v1p&&v2p&&!v3p&&v4p)||(!v1p&&!v2p&&v3p&&!v4p)) {
        cp1 = *v3+(*v1-*v3)*(-v3v/(v1v-v3v));
        cp2 = *v3+(*v2-*v3)*(-v3v/(v2v-v3v));
        cp3 = *v3+(*v4-*v3)*(-v3v/(v4v-v3v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((v1p&&v2p&&v3p&&!v4p)||(!v1p&&!v2p&&!v3p&&v4p)) {
        cp1 = *v4+(*v1-*v4)*(-v4v/(v1v-v4v));
        cp2 = *v4+(*v2-*v4)*(-v4v/(v2v-v4v));
        cp3 = *v4+(*v3-*v4)*(-v4v/(v3v-v4v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
    } else if ((!v1p&&!v2p&&v3p&&v4p)||(v1p&&v2p&&!v3p&&!v4p)) {
        cp1 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp2 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        cp3 = *v2+(*v3-*v2)*(-v2v/(v3v-v2v));
        cp4 = *v2+(*v4-*v2)*(-v2v/(v4v-v2v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4)); 
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4));    
    } else if ((!v1p&&v2p&&!v3p&&v4p)||(v1p&&!v2p&&v3p&&!v4p)) {        
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v4-*v1)*(-v1v/(v4v-v1v));
        cp3 = *v3+(*v2-*v3)*(-v3v/(v2v-v3v));
        cp4 = *v3+(*v4-*v3)*(-v3v/(v4v-v3v));           
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4));
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4)); 
    } else if ((!v1p&&v2p&&v3p&&!v4p)||(v1p&&!v2p&&!v3p&&v4p)) {
        cp1 = *v1+(*v2-*v1)*(-v1v/(v2v-v1v));
        cp2 = *v1+(*v3-*v1)*(-v1v/(v3v-v1v));
        cp3 = *v4+(*v2-*v4)*(-v4v/(v2v-v4v));
        cp4 = *v4+(*v3-*v4)*(-v4v/(v3v-v4v));
        triangles->push_back(Triangle(cp1, cp2, cp3));
        triangles->push_back(Triangle(cp2, cp3, cp4));
        //(&tris2)->push_back(Triangle(cp1, cp2, cp3));
        //(&tris2)->push_back(Triangle(cp2, cp3, cp4)); 
    }
}

/*
//               6       7         
//              +------+
//          5 .'|  8 .'|
//          +---+--+'  |
//          |   |  |   |           Vertices as labelled
//          |  2+--+---+3
//          | .'   | .'
//          +------+'
//          1       4 */

void Grid::addTriangles(vector_tri *triangles, float (*func)(Vector3f)) {
    for (int i = 0; i < numSteps(0)-1; i++) {
        for (int j = 0; j < numSteps(1)-1; j++) {
            for (int k = 0; k < numSteps(2)-1; k++) {
                int ic = numSteps(1)*numSteps(2);
                int jc = numSteps(2);

                Vector3f *v1 = &vertices[i*ic+j*jc+k];
                Vector3f *v2 = &vertices[i*ic+j*jc+k+1]; 
                Vector3f *v3 = &vertices[(i+1)*ic+j*jc+k+1];
                Vector3f *v4 = &vertices[(i+1)*ic+j*jc+k];
                Vector3f *v5 = &vertices[i*ic+(j+1)*jc+k];
                Vector3f *v6 = &vertices[i*ic+(j+1)*jc+k+1];
                Vector3f *v7 = &vertices[(i+1)*ic+(j+1)*jc+k+1];
                Vector3f *v8 = &vertices[(i+1)*ic+(j+1)*jc+k];

                Tetrahedron t1(v2, v1, v5, v4);
                Tetrahedron t2(v2, v4, v5, v8);
                Tetrahedron t3(v2, v5, v6, v8);
                Tetrahedron t4(v3, v6, v7, v8);
                Tetrahedron t5(v3, v2, v4, v8);
                Tetrahedron t6(v3, v2, v6, v8);

                //Tetrahedron t1(v5, v6, v8, v1);
                //Tetrahedron t2(v7, v6, v3, v8);
                //Tetrahedron t3(v2, v1, v6, v3);
                //Tetrahedron t4(v4, v8, v3, v1);
                //Tetrahedron t5(v1, v3, v6, v8);
                t1.addTriangles(triangles, func);
                t2.addTriangles(triangles, func);
                t3.addTriangles(triangles, func);
                t4.addTriangles(triangles, func);
                t5.addTriangles(triangles, func);
                t6.addTriangles(triangles, func);
            }
        }
    }   
}

// Only works for vectors with nonzero components
Grid::Grid(Vector3f center, Vector3f step, Vector3f gridMax) {
    Vector3f shiftToCenter = center - gridMax / 2;
    this->numSteps= Vector3i(gridMax(0)/step(0), gridMax(1)/step(1), gridMax(2)/step(2)) + Vector3i(1, 1, 1);
    for (int i = 0; i < numSteps(0); i++) {
        for (int j = 0; j < numSteps(1); j++) {
            for (int k = 0; k < numSteps(2); k++) {
                vertices.push_back(Vector3f(i*step(0), j*step(1), k*step(2)) + shiftToCenter);
            }
        }
    }
}

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

GLuint LoadTextureFromPNG(const char * filename, unsigned alpha) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; 
    lodepng::State state; //optionally customize this one
    unsigned int w, h;

    lodepng::load_file(png, filename); 
    
    unsigned error = lodepng::decode(image,  w,  h, state, png); // decode file into image
    if(error) std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    
    //alpha
    if (alpha < 255) {
        for(unsigned y = 0; y < h; y++) {
            for(unsigned x = 0; x < w; x++) {
                image[4 * w * y + 4 * x + 3] = alpha;
            }
        }
    }

    //void * ptr = &image[0];
    return AddTextureToOpenGL(w, h, &image[0]);

}
