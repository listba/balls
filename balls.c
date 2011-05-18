#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
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

#include "balls.h"

int width;
int height;
Ball *balls;
int numBalls = 0;
int rightDown = 0;
GLfloat rightX = 0.0;
GLfloat rightY = 0.0;
int wireframe = 0;
int gravity = 1;
int collision = 1;
int fps = 0;

int main(int argc, char **argv, char **envp) {
   width=START_WIDTH; height=START_HEIGHT;
   balls = malloc(sizeof(Ball)*MAX_BALLS);
   srand ( time(NULL) );
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (START_WIDTH, START_HEIGHT);
   int centerX = (glutGet(GLUT_SCREEN_WIDTH)-width)/2;
   int centerY = (glutGet(GLUT_SCREEN_HEIGHT)-height)/2;
   glutInitWindowPosition (centerX, centerY);
   glutCreateWindow ("Balls");
   InitGL();
   InitBalls();
   glutKeyboardFunc(ProcessKeys);
   glutDisplayFunc(DrawGLScene);
   glutReshapeFunc(Reshape);
   glutIdleFunc(Idle);
   glutMouseFunc(MouseClick);
   glutMainLoop();
   free(balls);
   return 0;
}

GLvoid InitGL() {
   glClearColor(0.0, 0.0, 0.0, 0.0);
   // Setup a 2d projection
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   glOrtho (0, width, height, 0, 0, 1);
   glDisable(GL_DEPTH_TEST);
   glMatrixMode (GL_MODELVIEW);
   //glTranslatef(0.375, 0.375, 0);
}

GLvoid ProcessKeys(unsigned char key, int x, int y) {
   switch(key) {
      case 'w':
         if(wireframe == 0) wireframe = 1;
         else wireframe = 0;
         break;
      case 'r':
         InitBalls(); 
         break;
      case 'g':
         if(gravity == 0) gravity = 1;
         else gravity = 0;
         break;
      case 'c':
         if(collision == 0) collision = 1;
         else collision = 0;
         break;
   }
}

GLvoid MouseClick(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON) {
      if (state == GLUT_DOWN) {
         rightDown = 1;
      } else {
         rightDown = 0;
      }
   }
}

GLvoid InitBalls() {
   numBalls = 0;
   int i;
   for (i=0; i<MAX_BALLS; i++) {
      balls[i].enabled = 0;
   }
   for (i=0; i<START_BALLS; i++) {
      AddRandBall();
   }
}

GLvoid AddBall(Ball ball) {
   int i = numBalls%MAX_BALLS;
   balls[i] = ball;
   numBalls++;
   UpdateTitle();
}

Ball CreateBall(GLfloat x, GLfloat y, GLfloat r) {
   Color col;
   Ball newBall;
   col.r=rand()%11/10.0;col.g=rand()%11/10.0;col.b=rand()%11/10.0;
   newBall.col = col;
   newBall.r = r;
   newBall.x = x;
   newBall.y = y;
   newBall.velX = 0.0;
   newBall.velY = 0.0;
   newBall.enabled = 1;
   newBall.elasticity = rand()%7/10.0 + 0.3;
   return newBall;
}

// Adds a ball in an unused location
GLvoid AddRandBall() {
   int startX; int startY;
   int alreadyThere=1;
   int radius;
   Ball newBall;
   while(alreadyThere==1) {
      alreadyThere = 0;
      radius = rand()%10+5;
      
      startX = rand()%(width-radius*2) + radius;
      startY = rand()%(height-radius*2) + radius;
      newBall = CreateBall(startX, startY, radius);
      int i=0;
      for (i=0; i<MAX_BALLS; i++) {
         if (balls[i].enabled == 1) {
            // collision
            if (CheckBallCollision(balls[i], newBall) == 1) {
               alreadyThere = 1; 
            }
         }
      }
   }
   AddBall(newBall);
}

GLvoid Idle() {
   static int frames = 0;
   static int lastUpdate = 0;
   static int lastTime= 0;
   glutPostRedisplay();
   int currentTime = glutGet( GLUT_ELAPSED_TIME );   
   frames++;
   if (rightDown==1) {
      AddRandBall();
   }
   
   if (( currentTime - lastUpdate ) >= 1000 ){
      fps = frames;
      UpdateTitle();
      frames = 0;
      lastUpdate = currentTime;
   }
   usleep(10000);
}

GLvoid UpdateTitle() {
   char buf[40];
   if (numBalls > MAX_BALLS) sprintf( buf, "Balls FPS: %d Balls:%d", fps, MAX_BALLS );
   else sprintf( buf, "FPS: %d Balls:%d", fps, numBalls );
   glutSetWindowTitle( buf );
}

GLvoid DrawGLScene() {
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();
   Color col;
   static int lastTime = 0;
   int deltaTime = 0;
   int currentTime = glutGet( GLUT_ELAPSED_TIME );
   deltaTime = currentTime-lastTime;
   lastTime = currentTime;
   // Render the balls   
   int i;
   for (i=0; i<MAX_BALLS; i++) {
      if (balls[i].enabled==1) {
         if (gravity==1) {
            balls[i].velY += ACCELERATION;
         }
         if (collision == 1) {
            int a;
            for (a=0; a<MAX_BALLS; a++) {
               if (CheckBallCollision(balls[a], balls[i]) && a != i) {
                  balls[i].velX = 0;
                  balls[i].velY = 0;
                  
                  break;
               }
            }
         }
         if (CheckHeightCollision(balls[i]) == 1) {
            balls[i].velY *= -1*balls[i].elasticity;
         }
         
         if (CheckWidthCollision(balls[i]) == 1) {
            balls[i].velX *= -1*balls[i].elasticity;
         }
         balls[i].x += balls[i].velX*deltaTime;
         balls[i].y += balls[i].velY*deltaTime;
         DrawBall(balls[i]);
      }
   }
   glutSwapBuffers();
}

GLvoid DrawBall(Ball ball) {
   double a;
   int x=ball.x;
   int y=ball.y;
   int radius=ball.r;
   GLfloat velX=ball.velX;
   GLfloat velY=ball.velY;
   Color col=ball.col;
   glColor3f(col.r, col.g, col.b);
   if (wireframe) glBegin(GL_LINE_STRIP); // Outline balls
   else glBegin(GL_TRIANGLE_FAN); // Solid balls
   for (a=0.0; a<360; a+= SMOOTHNESS) {
      glVertex2f(x + sin(a) * radius, y + cos(a) * radius);
   }
   glEnd();
}

GLvoid Reshape(int w, int h) {
}

int CheckBallCollision(Ball ball1, Ball ball2) {
   int dx = ball1.x - ball2.x;
   int dy = ball1.y - ball2.y;
   int radii = ball1.r + ball2.r;
   if ( ( dx * dx )  + ( dy * dy ) < radii * radii ) {
      return 1;
   } else {
      return 0;
   }
}

int CheckHeightCollision(Ball ball) {
   if (ball.y > height-ball.r && ball.velY > 0) {
      return 1;
   }
   if (ball.y < ball.r && ball.velY < 0) {
      return 1;
   }
   return 0;
}

int CheckWidthCollision(Ball ball) {
   if (ball.x > width-ball.r && ball.velX > 0) {
      return 1;
   }
   if (ball.x < ball.r && ball.velX < 0) {
      return 1;
   }
   return 0;
}
