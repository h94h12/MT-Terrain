CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -O7 -Wall -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-L"/opt/local/lib" -lGL -lGLU -lm -lstdc++ -lpthread
else
	CFLAGS = -g -O2 -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lGL -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: as3.o glUtil.o terrain.o lodepng.o Sky.o Sun.o Camera.o
	$(CC) $(CFLAGS) -o final glUtil.o terrain.o as3.o lodepng.o Camera.o Sky.o Sun.o $(LDFLAGS)
glUtil.o: glUtil.cpp
	$(CC) $(CFLAGS) -c glUtil.cpp -o glUtil.o
as3.o: as3.cpp
	$(CC) $(CFLAGS) -c as3.cpp -o as3.o	
terrain.o: terrain.cpp
	$(CC) $(CFLAGS) -c terrain.cpp -o terrain.o
lodepng.o: textures/lodepng.cpp
	$(CC) $(CFLAGS) -c textures/lodepng.cpp -o lodepng.o
Sky.o: Sky.cpp
	$(CC) $(CFLAGS) -c Sky.cpp -o Sky.o
Sun.o: Sun.cpp
	$(CC) $(CFLAGS) -c Sun.cpp -o Sun.o
Camera.o: Camera.cpp
	$(CC) $(CFLAGS) -c Camera.cpp -o Camera.o


clean: 
	$(RM) *.o as3
