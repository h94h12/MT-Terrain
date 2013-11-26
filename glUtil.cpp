#include "glUtil.h"

int counter = 0;
Vector3f tempNormal = Vector3f(0, 1, 0); 


bool std::operator==(const PairVecP &l, const PairVecP &r) {
        return ((l.first==r.first)&&(l.second==r.second))||((l.first==r.second)&&(l.second==r.first));
}

bool std::tr1::operator==(const PairVecP &l, const PairVecP &r) {
        return ((l.first==r.first)&&(l.second==r.second))||((l.first==r.second)&&(l.second==r.first));
}

size_t std::tr1::hash<PairVecP>::operator()(PairVecP const &key) const {
        std::size_t v1 = (std::size_t)key.first;
        std::size_t v2 = (std::size_t)key.second;

        v1 = ~v1 + (v1 << 15);
        v1 = v1 ^ (v1 >> 12);
        v1 = v1 + (v1 << 2);
        v1 = v1 ^ (v1 >> 4);
        v1 = v1 * 2057;
        v1 = v1 ^ (v1 >> 16);

        v2 = ~v2 + (v2 << 15);
        v2 = v2 ^ (v2 >> 12);
        v2 = v2 + (v2 << 2);
        v2 = v2 ^ (v2 >> 4);
        v2 = v2 * 2057;
        v2 = v2 ^ (v2 >> 16);

        return v1^v2;
}

bool Edge::findCP(float (*f)(Vector3f), Vector3f &intersection) {
        float v1d = f(*v1), v2d = f(*v2);
        bool v1pt = (v1d > 0);
        bool v2pt = (v2d > 0);
        bool v1nt = (v1d < 0);
        bool v2nt = (v2d < 0);
        if ((v1pt&&v2pt)||(v2nt&&v1nt)) {
                return false;
        } else {
                //intersection = bisection(f, *v1, *v2);
                intersection = *v1 + (*v2 - *v1)*(-v1d / (v2d - v1d));
                return true;
        }
}

Vector3f Edge::bisection(float (*f)(Vector3f), Vector3f left, Vector3f right) {
	Vector3f mid = (right + left)/2.0;
	float leftV = f(left); float midV = f(mid); float rightV = f(right);
	if (abs(midV) < epsilon) {		
		return mid;
	} else if (abs(leftV) < 1e-20) { 
		return left;
	} else if (abs(rightV) < 1e-20) {
    	return right;
    } else if (midV >= 0) {
        if (leftV >= 0) {
        	fflush(stdout);
        	return bisection(f, mid, right);
        } else {
        	fflush(stdout);
        	return bisection(f, left, mid);
        }
    } else {
     	if (leftV >= 0) {
          	return bisection(f, left, mid);
        } else {
    		fflush(stdout);            	
        	return bisection(f, mid, right);
        }
    }
}


Edge::Edge() {
        this->v1 = NULL;
        this->v2 = NULL;
   
}

Edge::Edge(Vector3f *v1, Vector3f *v2) {
        this->v1 = v1;
        this->v2 = v2;
}

Tetrahedron::Tetrahedron(Vector3f *v1, Vector3f *v2, Vector3f *v3, Vector3f *v4, Grid *grid) {
        this->v1 = v1; this->v2 = v2; this->v3 = v3; this->v4 = v4; this->grid = grid;
        Vector3f cp12, cp13, cp14, cp23, cp24, cp34;
        CPC = 0;
        //bool z1, z2, z3, z4, z5, z6;
        if (grid->edges[PairVecP(v1, v2)].findCP(grid->func, cp12)) {
                cutpoints[CPC] = cp12;
                CPC++;
        }
        if (grid->edges[PairVecP(v1, v3)].findCP(grid->func, cp13)) {
                cutpoints[CPC] = cp13;
                CPC++;
        }
        if (grid->edges[PairVecP(v1, v4)].findCP(grid->func, cp14)) {
                cutpoints[CPC] = cp14;
                CPC++;
        }
        if (grid->edges[PairVecP(v2, v3)].findCP(grid->func, cp23)) {
                cutpoints[CPC] = cp23;
                CPC++;
        }
        if (grid->edges[PairVecP(v2, v4)].findCP(grid->func, cp24)) {
                cutpoints[CPC] = cp24;
                CPC++;
        }
        if (grid->edges[PairVecP(v3, v4)].findCP(grid->func, cp34)) {
                cutpoints[CPC] = cp34;
                CPC++;
        }                                        
}

// Only works for vectors with nonzero components
Grid::Grid(Vector3f center, Vector3f step, Vector3f gridMax, float (*func)(Vector3f)) {
        this->center = center; this->step = step; this->gridMax = gridMax;
        Vector3f shiftToCenter = center - gridMax / 2;
        Vector3i numSteps= Vector3i(gridMax(0)/step(0), gridMax(1)/step(1), gridMax(2)/step(2)) + Vector3i(1, 1, 1);
        this->func = func;

        for (int i = 0; i < numSteps(0); i++) {
                for (int j = 0; j < numSteps(1); j++) {
                        for (int k = 0; k < numSteps(2); k++) {
                                vertices.push_back(Vector3f(i*step(0), j*step(1), k*step(2)) + shiftToCenter);
                        }
                }
        }
//        6       7         
//              +------+
//          5 .'|  8 .'|
//          +---+--+'  |
//    |   |  |   |           Vertices as labelled
//    |  2+--+---+3
//           | .'   | .'
//           +------+'
//    1       4

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

                                edges[PairVecP(v5, v6)] = Edge(v5, v6);
                                edges[PairVecP(v5, v8)] = Edge(v5, v8);
                                edges[PairVecP(v5, v1)] = Edge(v5, v1);
                                edges[PairVecP(v1, v8)] = Edge(v1, v8);
                                edges[PairVecP(v1, v6)] = Edge(v1, v6);
                                edges[PairVecP(v6, v8)] = Edge(v6, v8);                

                                edges[PairVecP(v4, v1)] = Edge(v4, v1);
                                edges[PairVecP(v4, v3)] = Edge(v4, v3);
                                edges[PairVecP(v4, v8)] = Edge(v4, v8);
                                edges[PairVecP(v3, v8)] = Edge(v3, v8);
                                edges[PairVecP(v1, v3)] = Edge(v1, v3);

                                edges[PairVecP(v7, v3)] = Edge(v7, v3);
                                edges[PairVecP(v7, v8)] = Edge(v7, v8);
                                edges[PairVecP(v7, v6)] = Edge(v7, v6);
                                edges[PairVecP(v3, v6)] = Edge(v3, v6);        

                                edges[PairVecP(v2, v1)] = Edge(v2, v1);
                                edges[PairVecP(v2, v3)] = Edge(v2, v3);
                                edges[PairVecP(v2, v6)] = Edge(v2, v6);
 
                                Tetrahedron t1(v5, v6, v8, v1, this);
                                Tetrahedron t2(v7, v6, v8, v3, this);
                                Tetrahedron t3(v2, v6, v1, v3, this);
                                Tetrahedron t4(v4, v1, v8, v3, this);
                                Tetrahedron t5(v1, v6, v3, v8, this);

                                if (t1.CPC > 2)
                                        tets.push_back(t1);
                                if (t2.CPC > 2)
                                        tets.push_back(t2);
                                if (t3.CPC > 2)
                                        tets.push_back(t3);
                                if (t4.CPC > 2)
                                        tets.push_back(t4);
                                if (t5.CPC > 2)
                                        tets.push_back(t5);
                        }
                }
        }
}


Vector3f getNormal(Vector3f v1, Vector3f v2, Vector3f v3){
    Vector3f v13 = v1 - v3; 
    Vector3f v23 = v2 - v3; 
    
    //return (RtimesSinv * v13.crossProduct(v23)).normalize();
    return v13.cross(v23).normalized();
}

void Tetrahedron::draw() {
        counter++;
        if (drawTets) {
                if (wireframe) {
                        glBegin(GL_LINE_LOOP);
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));
                        glEnd();

                        glBegin(GL_LINE_LOOP);
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));
                        glEnd();

                        glBegin(GL_LINE_LOOP);
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));                                
                        glEnd();

                        glBegin(GL_LINE_LOOP);
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                        glEnd();
                } else {
                        glBegin(GL_TRIANGLES);
        
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));
                
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));
                
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                        glVertex3f((*v3)(0), (*v3)(1), (*v3)(2));                                
                                                        
                        glVertex3f((*v1)(0), (*v1)(1), (*v1)(2));
                        glVertex3f((*v2)(0), (*v2)(1), (*v2)(2));
                        glVertex3f((*v4)(0), (*v4)(1), (*v4)(2));
                                                        
                        glEnd();
                }
        } else if (CPC == 3) {
                if (wireframe) {
                        glBegin(GL_LINE_LOOP);
                } else {
                        glBegin(GL_TRIANGLES);
                }
            
             
                Vector3f n = getNormal(cutpoints[0], cutpoints[1], cutpoints[2]); 
                if(n.dot(tempNormal) < 0) n = -n; 
                
                glNormal3f(n(0), n(1), n(2)); 
                glVertex3f(cutpoints[0](0), cutpoints[0](1), cutpoints[0](2));
                  glNormal3f(n(0), n(1), n(2));
                glVertex3f(cutpoints[1](0), cutpoints[1](1), cutpoints[1](2));
                  glNormal3f(n(0), n(1), n(2));
                glVertex3f(cutpoints[2](0), cutpoints[2](1), cutpoints[2](2));
                glEnd();
        } else if (CPC == 4) {
                if (wireframe) {
                        glBegin(GL_LINE_LOOP);
                        glVertex3f(cutpoints[0](0), cutpoints[0](1), cutpoints[0](2));
                        glVertex3f(cutpoints[1](0), cutpoints[1](1), cutpoints[1](2));
                        glVertex3f(cutpoints[2](0), cutpoints[2](1), cutpoints[2](2));
                        glEnd();

                        glBegin(GL_LINE_LOOP);
                        glVertex3f(cutpoints[3](0), cutpoints[3](1), cutpoints[3](2));        
                        glVertex3f(cutpoints[1](0), cutpoints[1](1), cutpoints[1](2));
                        glVertex3f(cutpoints[2](0), cutpoints[2](1), cutpoints[2](2));                        
                        glEnd();
                } else {
                        glBegin(GL_TRIANGLES);
                        Vector3f n = getNormal(cutpoints[0], cutpoints[1], cutpoints[2]); 
                        if(n.dot(tempNormal) < 0) n = -n; 
                
                        glNormal3f(n(0), n(1), n(2)); 
                        glVertex3f(cutpoints[0](0), cutpoints[0](1), cutpoints[0](2));
                        glNormal3f(n(0), n(1), n(2)); 
                        glVertex3f(cutpoints[1](0), cutpoints[1](1), cutpoints[1](2));
                        glNormal3f(n(0), n(1), n(2)); 
                        glVertex3f(cutpoints[2](0), cutpoints[2](1), cutpoints[2](2));

                        n = getNormal(cutpoints[1], cutpoints[2], cutpoints[3]); 
                        if(n.dot(tempNormal) < 0) n = -n; 
                
                        glNormal3f(n(0), n(1), n(2)); 
                        glVertex3f(cutpoints[3](0), cutpoints[3](1), cutpoints[3](2));     
                        glNormal3f(n(0), n(1), n(2));
                        glVertex3f(cutpoints[1](0), cutpoints[1](1), cutpoints[1](2));
                        glNormal3f(n(0), n(1), n(2));
                        glVertex3f(cutpoints[2](0), cutpoints[2](1), cutpoints[2](2));
                        glEnd();
                }                
        }
}

void Grid::draw() {
        for (vector<Tetrahedron>::iterator it = tets.begin(); it != tets.end(); ++it) {
                it->draw();
        }
}
