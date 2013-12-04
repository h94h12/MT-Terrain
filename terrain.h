#ifndef TerrainH
#define TerrainH
#include <cstdlib>
#include <time.h>
#include <math.h> 
#include <cstdio>
#define _USE_MATH_DEFINES

//http://www.float4x4.net/index.php/2010/06/generating-realistic-and-playable-terrain-height-maps/
class Perlin{
    
    static const int gradTableSize = 256; 
    
    public: 
    float gradients[256*3];    
    int perms[256]; 
    
    Perlin(); 
    
    float Noise(float, float, float); 
    void InitGradients(); 
    int Permutate(int); 
    int Index(int, int, int); 
    float Lattice(int, int, int, float, float, float); 
    float Lerp(float, float, float);
    float Smooth(float); 
    
}; 



class HeightMap{
    public:
    float* heights; 
    int size; 
    int max_height; 
    Perlin perlin; 
    
    HeightMap(){}; 
    HeightMap(int); 
    
    void addPerlinNoise(float); 
    void perturb(float, float);
    void erode(float); 
    void smoothen(); 
    
    
};

#endif 