#include "Cylinder.h"
#include <math.h>


/**
* Cylinder's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(Vector pos, Vector dir) {
  // printf("Actually doing some shit");
  float a = pow(dir.x, 2) + pow(dir.z, 2);
  float b = 2 * ((dir.x * (pos.x - origin.x)) + (dir.z *(pos.z - origin.z)));
  float c = pow((pos.x - origin.x), 2) + pow((pos.z - origin.z), 2) - pow(radius, 2);

  float desc = pow(b, 2) - 4 * a * c;

  if (fabs(desc) < 0.001 || desc < 0.0) {
    return -1.;
  }
  float t1 = (-b - sqrt(desc)) / (2 * a);
  float t2 = (-b + sqrt(desc)) / (2 * a);

  float y1 = pos.y + (t1 * dir.y);
  float y2 = pos.y + (t2 * dir.y);

  if (y1 > origin.y + height || y1 < origin.y) {
    if (y2 > origin.y + height || y2 < origin.y){
      return -1.;
    } else {
      return t2;
    }
  } else {
    return t1;
  }

	return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cylinder.
*/
Vector Cylinder::normal(Vector p)
{
  float x = (p.x - origin.x);
  float z = (p.z - origin.z);
  Vector n = Vector(x, 0, z);
  n.normalise();
  return n;
}
