all:
	gcc balls.c -o balls -framework OpenGL -framework GLUT
clean:
	rm -rf *o balls
