CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -Wall -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -std=c++11 -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lGL -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: as3.o glUtil.o
	$(CC) $(CFLAGS) -o as3 glUtil.o as3.o  $(LDFLAGS) 
glUtil.o: glUtil.cpp
	$(CC) $(CFLAGS) -c glUtil.cpp -o glUtil.o
as3.o: as3.cpp
	$(CC) $(CFLAGS) -c as3.cpp -o as3.o	

clean: 
	$(RM) *.o as3