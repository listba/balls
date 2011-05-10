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

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_BALLS = 1000;
const int START_BALLS = 100;
const GLfloat ACCELERATION = 0.004;
const GLfloat SMOOTHNESS = 1; // lower = more smooth balls

typedef struct {
   GLfloat r;
   GLfloat g;
   GLfloat b;
} Color;

typedef struct {
   Color col;
   GLfloat x;
   GLfloat y;
   GLfloat r;
   GLfloat velX;
   GLfloat velY;
   GLfloat elasticity;
   GLint enabled;
} Ball;

Ball *balls;
int numBalls = 0;
int rightDown = 0;
GLfloat rightX = 0.0;
GLfloat rightY = 0.0;
int wireframe = 0;
int gravity = 1;

GLvoid InitGL(GLvoid);
GLvoid DrawGLScene(GLvoid);
GLvoid Idle();
GLvoid ReSizeGLScene(int w, int h);
GLvoid DrawBall(Ball ball);
GLvoid MouseClick(int button, int state, int x, int y);
GLvoid MouseMove(int x, int y);
GLvoid ProcessKeys(unsigned char key, int x, int y);
GLvoid AddBall(GLfloat x, GLfloat y, GLfloat r);
GLvoid AddRandBall();
GLvoid InitBalls();
int CheckHeightCollision(Ball ball);
int CheckWidthCollision(Ball ball);

int main(int argc, char **argv, char **envp) {
   balls = malloc(sizeof(Ball)*MAX_BALLS);
   srand ( time(NULL) );
   InitBalls();
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (WIDTH, HEIGHT);
   int centerX = (glutGet(GLUT_SCREEN_WIDTH)-WIDTH)/2;
   int centerY = (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT)/2;
   glutInitWindowPosition (centerX, centerY);
   glutCreateWindow ("Balls");
   InitGL();
   glutMouseFunc(MouseClick);
   glutKeyboardFunc(ProcessKeys);
   glutDisplayFunc(DrawGLScene);
   glutReshapeFunc(ReSizeGLScene);
   // We might want +glutTimerFunc
   glutIdleFunc(Idle);
   glutMainLoop();
   free(balls);
   return 0;
}

GLvoid InitGL() {
   glClearColor(0.0, 0.0, 0.0, 0.0);
   // Setup a 2d projection
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   glOrtho (0, WIDTH, HEIGHT, 0, 0, 1);
   glDisable(GL_DEPTH_TEST);
   glMatrixMode (GL_MODELVIEW);
   glTranslatef(0.375, 0.375, 0);
}

GLvoid ProcessKeys(unsigned char key, int x, int y) {
   int i;
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

GLvoid AddBall(GLfloat x, GLfloat y, GLfloat r) {
   Color col;
   int i = numBalls%MAX_BALLS;
   col.r=rand()%11/10.0;col.g=rand()%11/10.0;col.b=rand()%11/10.0;
   balls[i].col = col;
   balls[i].r = r;
   balls[i].x = x;
   balls[i].y = y;
   balls[i].velX = 0.0;
   balls[i].velY = 0.0;
   balls[i].enabled = 1;
   balls[i].elasticity = rand()%7/10.0 + 0.3;
   numBalls++;
}

// Adds a ball in an unused location
GLvoid AddRandBall() {
   int startX; int startY;
   int alreadyThere=1;
   int radius;
   while(alreadyThere==1) {
      alreadyThere = 0;
      radius = rand()%10+5;
      
      startX = rand()%(WIDTH-radius*2) + radius;
      startY = rand()%(HEIGHT-radius*2) + radius;
      
      int sRight = startX + radius;
      int sLeft = startX - radius;
      int sDown = startY + radius;
      int sUp = startY - radius;
      int i=0;
      for (i=0; i<MAX_BALLS; i++) {
         if (balls[i].enabled == 1) {
            int iRight = balls[i].x + balls[i].r;
            int iLeft = balls[i].x - balls[i].r;
            int iDown = balls[i].y + balls[i].r;
            int iUp = balls[i].y - balls[i].r;
            // collision
            if (sRight > iLeft && sLeft <= iRight) {
               if (sDown > iUp && sUp <= iDown) {
                  alreadyThere = 1;
                  break;
               }
            }
         }
      }
   }
   AddBall(startX, startY, radius);
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
      char buf[40];
      if (numBalls > MAX_BALLS) sprintf( buf, "Balls FPS: %d Balls:%d", frames, MAX_BALLS );
      else sprintf( buf, "FPS: %d Balls:%d", frames, numBalls );
      frames = 0;
      lastUpdate = currentTime;
      glutSetWindowTitle( buf );
   }
   usleep(10000);
}

GLvoid DrawGLScene() {
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();
   Color col;
   int i;
   static int lastTime = 0;
   int deltaTime = 0;
   int currentTime = glutGet( GLUT_ELAPSED_TIME );
   deltaTime = currentTime-lastTime;
   lastTime = currentTime;
   
   for (i=0; i<MAX_BALLS; i++) {
      if (balls[i].enabled==1) {
         
         if (gravity==1) {
            balls[i].velY += ACCELERATION;
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

GLvoid ReSizeGLScene(int w, int h) {
}

int CheckHeightCollision(Ball ball) {
   if (ball.y > HEIGHT-ball.r && ball.velY > 0) {
      return 1;
   }
   if (ball.y < ball.r && ball.velY < 0) {
      return 1;
   }
   return 0;
}

int CheckWidthCollision(Ball ball) {
   if (ball.x > WIDTH-ball.r && ball.velX > 0) {
      return 1;
   }
   if (ball.x < ball.r && ball.velX < 0) {
      return 1;
   }
   return 0;
}
