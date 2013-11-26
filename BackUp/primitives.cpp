#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include <cstdlib>
#include <string.h>
#include <time.h>
#include <math.h>
#include "primitives.h"


//Constructor
Point::Point(double nx, double ny, double nz) {
		x = nx;
		y = ny;
		z = nz;
        u = 0;
        v = 0; 
}

Point::Point(double nx, double ny, double nz, double nu, double nv) {
		x = nx;
		y = ny;
		z = nz;
		u = nu; // parametric numbers,
		v = nv;
}

Vertex::Vertex(Point p, bool b){
    point = p; 
    sign = b; 
    
} 
//Constructor
Vector::Vector(double nx, double ny, double nz){
        dx = nx;
        dy = ny; 
        dz = nz; 
        mag = sqrt(nx*nx + ny*ny + nz*nz);
}
Vector::Vector(Point p){
       dx = p.x;
       dy =  p.y; 
       dz = p.z; 
       mag = sqrt(dx*dx + dy*dy + dz*dz);
}


//Construct vector from two points
//Vector goes from p1 to p2
Vector::Vector(Point p1, Point p2){
    dx = p2.x - p1.x; 
    dy = p2.y - p1.y;
    dz = p2.z - p1.z; 
    mag = sqrt(dx*dx + dy*dy + dz*dz);
}

Quad::Quad(Point a, Point b, Point c, Point d, Vector an, Vector bn, Vector cn, Vector dn){
    p0 = a, p1 = b, p2 = c, p3 = d; 
    n0 = an, n1 = bn, n2 = cn, n3 = dn; 
}

Triangle::Triangle(Point a, Point b, Point c, Vector an, Vector bn, Vector cn){
     p0 = a,  p1 = b,  p2 = c;
     n0 = an, n1 = bn, n2 = cn;
    
}
Triangle::Triangle(Point a, Point b, Point c){
    p0 = a,  p1 = b,  p2 = c;
}

Edge::Edge(Point point0, Point point1){
    p0 = point0;
    p1 = point1;
}

Point Edge::getMidpoint(){
    
    return (p0 + p1)*0.5; 
}

//****************************************************
// Point functions
//****************************************************



//PointA - PointB = vector from B to A
//return Vector that is result of this - p, which is from p to this
Vector Point:: subtract (Point p){
    return Vector(x - p.x, y - p.y, z - p.z); 
}

Vector Point::operator- (Point p){
    return Vector(x - p.x, y - p.y, z - p.z); 
}

Point Point::operator+ (Vector v){
    return Point(x + v.dx, y + v.dy, z + v.dz); 
}

// implemented only for the sake of camera; be wary of using normally....
Point Point::operator+ (Point p){
    return Point(x + p.x, y + p.y, z + p.z, u + p.u, v + p.v); 
}

Point Point::operator* (double s){
    return Point(x*s, y*s, z*s, u*s, v*s); // parametric not exactly scalar, but good enough for this 
}
//****************************************************
// Vector functions
//****************************************************

//normalize this vector
Vector Vector::normalize(){
    Vector v = Vector((double)dx / mag, (double)dy / mag, (double)dz / mag);  
    v.mag = 1;
    return v;
}
double Vector::dotProduct(Vector v){
    return dx * v.dx + dy * v.dy + dz * v.dz; 
}

//return negation of this vector
Vector Vector::negative(){
   return Vector(-dx, -dy, -dz); 
}

//multiply this vector by scalar
Vector Vector :: operator*(double a){
   return Vector(a*dx, a*dy, a*dz); 
}

//add to this vector to another vector
Vector Vector::operator+ (Vector v){
    return Vector(dx + v.dx, dy + v.dy, dz + v.dz); 
}
//subtract by another vector
Vector Vector::operator- (Vector v){
    return Vector(dx - v.dx, dy - v.dy, dz - v.dz); 
}


// takes the cross product of this vector and vector V.  Returns 
// in a vector which is perpendicular to both and therefore normal 
// to the plane containing them
Vector Vector::crossProduct(Vector v){
    return Vector(dy*v.dz - dz*v.dy,
				  dz*v.dx - dx*v.dz,
				  dx*v.dy - dy*v.dx);
   
    
}

Tet::Tet(Vertex v_0, Vertex v_1, Vertex v_2, Vertex v_3){
    v0 = v_0; 
    v1 = v_1; 
    v2 = v_2; 
    v3 = v_3; 
}



