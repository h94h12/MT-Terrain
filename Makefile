CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX -O2
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: primitives.o marchingtet.o 
	$(CC) $(CFLAGS) -o marchingtet primitives.o marchingtet.o  $(LDFLAGS) 
marchingetet.o: marchingtet.cpp
	$(CC) $(CFLAGS) -c marchingtet.cpp -o marchingtet.o
primitives.o: primitives.cpp
	$(CC) $(CFLAGS) -c primitives.cpp -o primitives.o

clean: 
	$(RM) *.o primitives marchingtet
 
 


