#ifndef POINT_H
#define POINT_H

class point {
public:
   point(float cx, float cy);
   point();
   float distance(point otherpoint);
   point & operator+=(const point &rhs);
   const point operator+(const point &other) const;
   point & operator-=(const point &rhs);
   const point operator-(const point &other) const;
   point & operator*=(const int &rhs);
   const point operator*(const int &other) const;
   float x;
   float y;
};

#endif
