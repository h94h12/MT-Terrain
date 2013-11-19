
#include <cstdlib>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;


//****************************************************
// CLASS DECLARATIONS AND CONSTRUCTORS 
//****************************************************]
class Vector; 
class Point{ 
public:
    Point() {};
    Point(double, double, double); 
    Point(double, double, double, double, double); 

    double x, y, z, u, v;
    Vector subtract(Point); 
    Point operator + (Vector); //position + direction = position 
    Point operator + (Point); // point addition; shortcut for camera's ray formula
    Point operator * (double); 
    Vector operator - (Point);  
    
    
    
};
    
class Vector{
 public:
    Vector() {};  
    Vector(double, double, double);
    Vector(Point); 
    Vector(Point, Point); 
    
    double dx, dy, dz;
    double mag;
    
    Vector normalize();
    double dotProduct(Vector);
    Vector negative();
    Vector crossProduct(Vector);
    
    //allow for scalar * and vector +
    Vector operator * (double);
    Vector operator + (Vector); 
    Vector operator - (Vector); 
    
    
};

class Quad{
  public:
        Quad(){}; 
        Point p0, p1, p2, p3; 
        Vector n0, n1, n2, n3; 
        Quad(Point, Point, Point, Point, Vector, Vector, Vector, Vector); 
    
};

class Triangle{
    public:
        Triangle(){};
        Point p0, p1, p2; 
        Vector n0, n1, n2; 
        Triangle(Point, Point, Point); 
        Triangle(Point, Point, Point, Vector, Vector, Vector);  
    
}; 

class Vertex{
    public:
    Vertex(){}; 
    Vertex(Point, bool); 
    Point point;
    bool sign; // - outside,   + inside,  + on surface
};

class Tet{
    public:
    Tet(){}; 
    Tet(Vertex, Vertex, Vertex, Vertex); 
    Vertex v0, v1, v2, v3; 
}; 

class Cube{
    public:
    Cube(){};
    Vertex FLT, FLB, FRT, FRB,   NLT, NLB, NRT, NRB;
};





class Edge{
    public:
        Point p0;
        Point p1;
        
        Edge(){}; 
        Edge(Point, Point);
        
        Point getMidpoint();
    
}; 


