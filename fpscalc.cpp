#include "fpscalc.h"
fpscalc::fpscalc() {
   fps=0;
   frames=0;
   lastupdate=0;
}
int fpscalc::getfps() {
   return fps;
}
void fpscalc::tick() {
   int curtime = glutGet( GLUT_ELAPSED_TIME );
   frames++;
   if (( curtime - lastupdate ) >= 1000 ) {
      fps = frames;
      frames = 0;
      lastupdate = curtime;
   }
}

