#include "Camera.h"

unsigned char *arr = NULL;

void initCamera() {
	if (arr != NULL) {
		delete arr;
	}
	arr = new unsigned char[viewport.h*viewport.w*4];
}

void invertIMG(int width, int height) {
	unsigned char *rep = new unsigned char[viewport.h*viewport.w*4];
	width = width*4;
	height = height;
	for (int i = 0; i < (height+2) / 2; i++) {
		for (int j = 0; j < width; j++) {
			rep[i*width + j] = arr[(height-i)*width + j];
			rep[(height-i)*width +j] = arr[i*width + j];
		}
	}
	delete arr;
	arr = rep;
}

void takePicture() {
	glReadPixels(0, 0, viewport.w-1, viewport.h-1, GL_RGBA, GL_UNSIGNED_BYTE, arr);
	invertIMG(viewport.w-1, viewport.h);	
}

void writeImage(const char * filename) {
	unsigned error = lodepng::encode(std::string(filename), arr, viewport.w-1, viewport.h-1);	
}

void renderBlackandWhite() {
	int width = viewport.w-1;
	int height = viewport.h-1;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			unsigned char* loc = (arr + i*(width*4) + j*4);
			unsigned char mono = (unsigned char)(*(loc)*0.2125 + *(loc+1)*.7154 + *(loc+2)*.0721);
			*loc = mono; *(loc + 1) = mono; *(loc + 2) = mono;
		}
	}
}

// G B G
// R G R
// G B G 
void bayerFilter() {
	int width = (viewport.w-1)/3*3;
	int height = (viewport.h-1)/3*3;
	for (int i = 0; i < height; i+=3) {
		for (int j = 0; j < width; j+=3) {
			unsigned char* loc00 = (arr + (i+0)*(width*4) + (j+0)*4);
			unsigned char* loc01 = (arr + (i+0)*(width*4) + (j+1)*4);
			unsigned char* loc02 = (arr + (i+0)*(width*4) + (j+2)*4);					
			unsigned char* loc10 = (arr + (i+1)*(width*4) + (j+0)*4);
			unsigned char* loc11 = (arr + (i+1)*(width*4) + (j+1)*4);
			unsigned char* loc12 = (arr + (i+1)*(width*4) + (j+2)*4);
			unsigned char* loc20 = (arr + (i+2)*(width*4) + (j+0)*4);			
			unsigned char* loc21 = (arr + (i+2)*(width*4) + (j+1)*4);
			unsigned char* loc22 = (arr + (i+2)*(width*4) + (j+2)*4);
			*(loc00 + 0) = *(loc10);
			*(loc00 + 2) = *(loc01 + 2);
			*(loc01 + 0) = *(loc10)/2 + *(loc12)/2;
			*(loc01 + 1) = *(loc00 + 1)/3 + *(loc02 + 1)/3 + *(loc11 + 1)/3;
			*(loc02 + 0) = *(loc12);
			*(loc02 + 2) = *(loc01 + 2);
			*(loc10 + 1) = *(loc00 + 1)/3 + *(loc11 + 1)/3 + *(loc20 + 1)/3;
			*(loc10 + 2) = *(loc01 + 2)/2 + *(loc21 + 2)/2;
			*(loc11 + 0) = *(loc10)/2 + *(loc12)/2;
			*(loc11 + 2) = *(loc01 + 2)/2 + *(loc21 + 2)/2;
			*(loc12 + 1) = *(loc02 + 1)/3 + *(loc22 + 1)/3 + *(loc11 + 1)/3;
			*(loc12 + 2) = *(loc01 + 2)/2 + *(loc21 + 2)/2;
			*(loc20 + 0) = *loc10;
			*(loc20 + 2) = *(loc21 + 2);
			*(loc21 + 0) = *(loc10)/2 + *(loc12)/2;
			*(loc21 + 1) = *(loc20 + 1)/3 + *(loc11 + 1)/3 + *(loc22 + 1)/3;
			*(loc22 + 0) = *loc12;
			*(loc22 + 2) = *(loc21 + 2);
		} 
	}			
			
}
