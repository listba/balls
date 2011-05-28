#include "ball.h"

ball::ball(int cradius, color ccolor, point cposition) {
   radius = cradius;
   col = ccolor;
   position = cposition;
   solid = true;
   moved = false;
}

void ball::render() {
   glColor3f(col.r,col.g,col.b);
   if (solid) glBegin(GL_TRIANGLE_FAN);
   else glBegin(GL_LINE_LOOP);
   for (float i=0.0; i<360.0; i+= SMOOTH) {
      //glVertex2f(position.x+sin(i) * radius, position.y+cos(i) * radius);
      glVertex2f(sin(i) * radius, cos(i) * radius);
   }
   glEnd();
}

bool ball::iscolliding(ball otherball) {
   point otherpos = otherball.position;
   if (position.distance(otherpos) <= otherball.radius + radius) {
      return true;
   } else {
      return false;
   }
}
