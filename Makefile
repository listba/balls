LIBS = -lglut -lGLU -lGLEW
PLATFORM = LINUX

ifeq ($(PLATFORM), "x")
   LIBS = -framework OpenGl -framework GLUT
endif

all:
	gcc balls.c $(LIBS) -o balls
clean:
	rm -rf *o balls
