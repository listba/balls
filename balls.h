const int START_WIDTH = 800;
const int START_HEIGHT = 600;
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

GLvoid InitGL(GLvoid);
GLvoid DrawGLScene(GLvoid);
GLvoid Idle();
GLvoid Reshape(int w, int h);
GLvoid DrawBall(Ball ball);
GLvoid MouseClick(int button, int state, int x, int y);
GLvoid MouseMove(int x, int y);
GLvoid ProcessKeys(unsigned char key, int x, int y);
GLvoid AddBall(Ball ball);
GLvoid AddRandBall();
GLvoid InitBalls();
Ball CreateBall(GLfloat x, GLfloat y, GLfloat radius);
int CheckBallCollision(Ball ball1, Ball ball2);
int CheckHeightCollision(Ball ball);
int CheckWidthCollision(Ball ball);
GLvoid UpdateTitle();
