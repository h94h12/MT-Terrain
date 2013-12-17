#include "terrain.h"


/*
adapted from: 
http://www.float4x4.net/index.php/2010/06/generating-realistic-and-playable-terrain-height-maps/
 */
Perlin::Perlin(int seed){
    for(int i = 0; i < 256; i++) perms[i] = i; 
    int j;
    int temp; 
    srand(time(NULL)); //unreliable!!!!
    //srand(seed); 
    for(int i = 256-1; i > 1; i--){
        j = rand() % i;
        temp = perms[j]; 
        perms[j] = perms[i]; 
        perms[i] = temp; 
    }
    
    InitGradients(); 

}





float Perlin::Noise(float x, float y, float z) {
    int ix = (int)floorf(x);
    float fx0 = x - ix;
    float fx1 = fx0 - 1;
    float wx = Smooth(fx0);

    int iy = (int)floorf(y);
    float fy0 = y - iy;
    float fy1 = fy0 - 1;
    float wy = Smooth(fy0);

    int iz = (int)floorf(z);
    float fz0 = z - iz;
    float fz1 = fz0 - 1;
    float wz = Smooth(fz0);

    float vx0 = Lattice(ix, iy, iz, fx0, fy0, fz0);
    float vx1 = Lattice(ix + 1, iy, iz, fx1, fy0, fz0);
    float vy0 = Lerp(wx, vx0, vx1);

    vx0 = Lattice(ix, iy + 1, iz, fx0, fy1, fz0);
    vx1 = Lattice(ix + 1, iy + 1, iz, fx1, fy1, fz0);
    float vy1 = Lerp(wx, vx0, vx1);

    float vz0 = Lerp(wy, vy0, vy1);

    vx0 = Lattice(ix, iy, iz + 1, fx0, fy0, fz1);
    vx1 = Lattice(ix + 1, iy, iz + 1, fx1, fy0, fz1);
    vy0 = Lerp(wx, vx0, vx1);

    vx0 = Lattice(ix, iy + 1, iz + 1, fx0, fy1, fz1);
    vx1 = Lattice(ix + 1, iy + 1, iz + 1, fx1, fy1, fz1);
    vy1 = Lerp(wx, vx0, vx1);

    float vz1 = Lerp(wy, vy0, vy1);
    return Lerp(wz, vz0, vz1);
}
 
void Perlin::InitGradients(){
    for (int i = 0; i < gradTableSize; i++) {
        float z = 1 - 2 * (float)(rand() % 256); 
        float r = (float)sqrt(abs(1 - z * z));
        float theta = 2 * (float)M_PI *(float)rand(); 
        gradients[i * 3] = r * (float)cos(theta);
        gradients[i * 3 + 1] = r * (float)sin(theta);
        gradients[i * 3 + 2] = z;
    }
}
 
int Perlin::Permutate(int x){
    const int mask = gradTableSize - 1;
    return perms[x & mask];
}
 
int Perlin::Index(int ix, int iy, int iz){
    return Permutate(ix + Permutate(iy + Permutate(iz)));
}
 
float Perlin::Lattice(int ix, int iy, int iz, float fx, float fy, float fz){
    int index = Index(ix, iy, iz);
    int g = index * 3;
    return gradients[g] * fx + gradients[g + 1] * fy + gradients[g + 2] * fz;
}
 
float Perlin::Lerp(float t, float value0, float value1){
    return value0 + t * (value1 - value0);
}
 
float Perlin::Smooth(float x){
    return x * x * (3 - 2 * x);
}


HeightMap::HeightMap(int s, int seed){
    size = s;
    heights = new float[s*s]; 
    perlin = Perlin(seed); 
    for (unsigned x = 0; x < s; x++) {
        for (unsigned y = 0; y < s; y++) {
            heights[x*s + y] = 0;
        }   
    }
}



void HeightMap::addPerlinNoise(float f){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            heights[i*size + j] += perlin.Noise(f * i / (float)size, f * j / (float)size, 0);
        }
    }
}
 
void HeightMap::perturb(float f, float d) {
    int u, v;
    float* temp = new float[size * size];
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            u = i + (int)(perlin.Noise(f * i / (float)size, f * j / (float)size, 0) * d);
            v = j + (int)(perlin.Noise(f * i / (float)size, f * j / (float)size, 1) * d);
            if (u < 0) u = 0; if (u >= size) u = size - 1;
            if (v < 0) v = 0; if (v >= size) v = size - 1;
            temp[i*size + j] = heights[u*size + v];
        }
    }
    heights = temp;
}
 
void HeightMap::erode(float smoothness){
    for (int i = 1; i < size - 1; i++){
        for (int j = 1; j < size - 1; j++){
            float d_max = 0.0f;
            int match[2] = {0, 0};

            for (int u = -1; u <= 1; u++){
                for (int v = -1; v <= 1; v++) {
                    if(abs(u) + abs(v) > 0){
                        float d_i = heights[i*size + j] - heights[(i + u)*size + j + v];
                        if (d_i > d_max) {
                            d_max = d_i;
                            match[0] = u; match[1] = v;
                        }
                    }
                }
            }
 
            if(0 < d_max && d_max <= (smoothness / (float)size)) {
                float d_h = 0.5f * d_max;
                heights[i *size + j] -= d_h;
                heights[(i + match[0])*size + j + match[1]] += d_h;
            }
        }
    }
}
 
void HeightMap::smoothen(){
    for (int i = 1; i < size - 1; ++i){
        for (int j = 1; j < size - 1; ++j) {
            
            float total = 0.0f;
            for (int u = -1; u <= 1; u++) {
                for (int v = -1; v <= 1; v++){
                    total += heights[(i + u)*size + j + v];
                }
            }

            heights[i*size + j] = total / 9.0f;
        }
    }
}






















