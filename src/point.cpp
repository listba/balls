#include "point.h"
#include <cmath>

point::point(float cx, float cy) {
   x = cx;
   y = cy;
}

point::point() {
   x=0;
   y=0;
}

float point::distance(point otherpoint) {
   return sqrt(pow(otherpoint.x-x,2) + pow(otherpoint.y-y,2));
}

point & point::operator+=(const point &rhs) {
   x += rhs.x;
   y += rhs.y;
   return *this;
}

const point point::operator+(const point &rhs) const {
   return point(*this) += rhs;
}

point & point::operator-=(const point &rhs) {
   x -= rhs.x;
   y -= rhs.y;
   return *this;
}

const point point::operator-(const point &rhs) const {
   return point(*this) -= rhs;
}

point & point::operator*=(const int &rhs) {
   x *= rhs;
   y *= rhs;
   return *this;
}

const point point::operator*(const int &rhs) const {
   return point(*this) *= rhs;
}
