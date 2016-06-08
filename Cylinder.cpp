#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(Vector pos, Vector dir) {
  float a = pow(dir.x, 2) + pow(dir.z, 2);
  float b = (dir.x * (pos.x - origin.x)) + (dir.z *(pos.z - origin.z));
  float c = pow((pos.x - origin.x), 2) + pow((pos.z - origin.z), 2) - pow(radius, 2);

  float desc = pow(b, 2) - 4 * a * c;

  if (fabs(desc) > 0.001 || desc < 0.0) {
    return -1.;
  }
  float t1 = -b - sqrt(desc);
  float t2 = -b + sqrt(desc);
  if(fabs(t1) < 0.001 )
  {
    if (t2 > 0) return t2;
    else t1 = -1.0;
  }
  if(fabs(t2) < 0.001 ) t2 = -1.0;

	return (t1 < t2)? t1: t2;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cylinder.
*/
Vector Cylinder::normal(Vector p)
{
    Vector n = p / radius;
    n.y = 0;
    n.normalise();
    return n;
}
