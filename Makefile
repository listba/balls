LIBS = -lglut -lGLU -lGLEW
PLATFORM = LINUX

ifeq ($(PLATFORM), "x")
   LIBS = -framework OpenGl -framework GLUT
endif

all:
	g++ main.cpp $(LIBS) -o balls -std=c++0x
clean:
	rm -rf *o balls
