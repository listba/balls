#ifndef BALL_H
#define BALL_H

#include "point.h"
#include "color.h"

class ball {
private:
   static const float SMOOTH = 1.5;
   //static const float SMOOTH = .1;
public:
   color col;
   int radius;
   bool solid;
   point position;
   point velocity;
   bool moved;
   ball(int cradius, color ccolor, point cposition);
   bool iscolliding(ball ball1);
   void render();
};

#endif
