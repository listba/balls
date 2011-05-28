#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <queue>
#include <map>
#ifdef __APPLE__
   #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
   #include <GLUT/glut.h>
// Linux
#else
   #include <GL/gl.h>
   #include <GL/glu.h>
   #include <GL/glut.h>
#endif
#include "color.cpp"
#include "ball.cpp"
#include "point.cpp"
#include "constants.h"
#include "fpscalc.cpp"
#define CHECKS 3 
using namespace std;

// Function pronotypes
void InitGL();
void InitBalls(int radius, int numcol, int numrow);
void UpdateTitle();
void UpdateVelocities(int deltatime, float bounce);
void UpdateSideCollision(float bounce, ball &ball);
void UpdateBallCollision(float bounce, ball &ball);
void UpdateCollisions(float bounce);
void UpdatePositions();
int GetGridPosition(ball cball);
GLvoid DrawGLScene();
GLvoid Idle();
GLvoid Reshape(int w, int h);
GLvoid MouseClick(int button, int state, int x, int y);
GLvoid ProcessKeys(unsigned char key, int x, int y);
// Global variables
map<int, queue<ball>> grid;
queue<ball> balls;
int totalballs=0;
fpscalc fps;

int main(int argc, char *argv[]) {
   srand ( time(NULL) );
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (WIDTH, HEIGHT);
   int centerX = (glutGet(GLUT_SCREEN_WIDTH)-WIDTH)/2;
   int centerY = (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT)/2;
   glutInitWindowPosition (centerX, centerY);
   glutCreateWindow ("Balls");
   InitGL();
   InitBalls(15,5,10);
   glutKeyboardFunc(ProcessKeys);
   glutDisplayFunc(DrawGLScene);
   glutReshapeFunc(Reshape);
   glutIdleFunc(Idle);
   glutMouseFunc(MouseClick);
   glutMainLoop();
   return 0;
}

void InitBalls(int radius, int numcol, int numrow) {
   int diameter = radius * 2;
   int cx = WIDTH/2; 
   int ccx = cx-radius*numcol;
   int cy = HEIGHT/2;
   for (float x=radius+ccx; x<=(diameter+.1)*numcol+ccx; x += (diameter+.1)) {
      for (float y=radius; y<=(diameter+.1)*numrow; y += (diameter+.1)) {
         float r=rand()%11/10.0;float g=rand()%11/10.0;float b=rand()%11/10.0;
         color col(r,g,b);
         point pos(x,y);
         ball newball(radius,col,pos);
         balls.push(newball);
         totalballs++;
      }
   }
}

void InitGL() {
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   glOrtho (0, WIDTH, HEIGHT, 0, 0, 1);
   glMatrixMode (GL_MODELVIEW);
   glDisable(GL_DEPTH_TEST);
}

GLvoid DrawGLScene() {
   float ball_bounce = 0.9;
   float side_bounce = 0.3;
   //float bounce = 0;
   //float bounce = .65;
   static int prevtime=glutGet(GLUT_ELAPSED_TIME);
   int deltatime = glutGet(GLUT_ELAPSED_TIME) - prevtime;
   prevtime = glutGet(GLUT_ELAPSED_TIME);
   glLoadIdentity();
   glClear(GL_COLOR_BUFFER_BIT);

   // Update balls velocities
   UpdateVelocities(deltatime, side_bounce);
   // Check for collisions on the updated balls
   for (int i=0; i<CHECKS; i++) UpdateCollisions(ball_bounce);
   // Move balls
   UpdatePositions();
   glutSwapBuffers();
}
GLvoid Idle() {
   glutPostRedisplay();
   fps.tick();
   UpdateTitle();
   usleep(1000);
}
GLvoid Reshape(int w, int h) {
}
GLvoid MouseClick(int button, int state, int x, int y) {
}
GLvoid ProcessKeys(unsigned char key, int x, int y) {
}
void UpdateTitle() {
   char buf[40];
   sprintf( buf, "FPS: %d Balls: %d",fps.getfps(),totalballs);
   glutSetWindowTitle(buf); 
}

void UpdateVelocities(int deltatime, float bounce) {
   // Update balls velocities
   for (int i=0; i<balls.size(); i++) {
      ball curball = balls.front();
      curball.velocity.y += 0.01 * deltatime;
      glPushMatrix();
      glTranslatef(curball.position.x, curball.position.y, 0.0);
      curball.render();
      glPopMatrix();
      UpdateSideCollision(bounce, curball);
      balls.push(curball);
      balls.pop();
   }
}
void UpdateBallCollision(float bounce, ball &curball) {
   // Check for collisions on the updated balls
   for (int a=0; a<balls.size(); a++) {
      ball otherball = balls.front();
      if (curball.iscolliding(otherball)) {
         // Move balls so they are not intersecting
         point delta = curball.position - otherball.position;
         float d = sqrt(pow(delta.x,2) + pow(delta.y,2));
         float f = (((curball.radius + otherball.radius)-d)/d);
         //delta *= (((curball.radius + otherball.radius)-d)/d);
         float xfix = delta.x*f/2;
         float yfix = delta.y*f/2;

         curball.position.x += xfix;
         curball.position.y += yfix;
         otherball.position.x -= xfix;
         otherball.position.y -= yfix;
         // Update balls velocities
         float dx = curball.position.x - otherball.position.x;
         float dy = curball.position.y - otherball.position.y;
         float angle = atan(dx/dy);
         float u1 = sqrt(pow(curball.velocity.x,2) + pow(curball.velocity.y,2));
         float u2 = sqrt(pow(otherball.velocity.x,2) + pow(otherball.velocity.y,2));
         float m1 = curball.radius;
         float m2 = otherball.radius;
         float U1 = (((m1-m2)/(m1+m2))*u1) + (((2*m2)/(m1+m2))*u2);
         float U2 = (((m2-m1)/(m1+m2))*u2) + (((2*m1)/(m1+m2))*u1);
         curball.velocity.x = U1*(-1)*cos(angle)*bounce;
         curball.velocity.y = U1*(-1)*sin(angle)*bounce;
         otherball.velocity.x = U2*cos(angle)*bounce;
         otherball.velocity.y = U2*sin(angle)*bounce;
         curball.position += curball.velocity;
         otherball.position += otherball.velocity;
         otherball.moved = true;
         curball.moved = true;
      }
      balls.push(otherball);
      balls.pop();
   }
}
void UpdateCollisions(float bounce) {
   for (int i=0; i<balls.size(); i++) {
      ball curball = balls.front();
      balls.pop();
      UpdateBallCollision(bounce, curball);
      balls.push(curball);
   }
}
void UpdatePositions() {
   for (int i=0; i<balls.size(); i++) {
      ball curball = balls.front();
      if (!curball.moved) curball.position += curball.velocity;
      curball.moved = false;
      int gridpos = GetGridPosition(curball);
      balls.push(curball);
      balls.pop();
   }
}

int GetGridPosition(ball cball) {
   int x = cball.position.x;
   int y = cball.position.y;
   return (x/200) + (y/150)*4;
}

void UpdateSideCollision(float bounce, ball &curball) {
   // bottom screen
   if (curball.position.y > HEIGHT-curball.radius) {
      curball.position.y = HEIGHT-curball.radius;
      curball.velocity.y *= -bounce;
   }
   // top screen
   if (curball.position.y < curball.radius) {
      curball.position.y = curball.radius;
      curball.velocity.y *= -bounce;
   }
   // left screen
   if (curball.position.x < curball.radius) {
      curball.position.x = curball.radius;
      curball.velocity.x *= -bounce;
   }
   // right screen
   if (curball.position.x > WIDTH-curball.radius) {
      curball.position.x = WIDTH-curball.radius;
      curball.velocity.x *= -bounce;
   }
}
