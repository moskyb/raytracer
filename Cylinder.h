#ifndef H_CYLINDER
#define H_CYLINDER

#include "Object.h"

/**
 * Defines a simple Cylinder located at 'origin'
 * with the specified radius
 */
class Cylinder : public Object
{

private:
    Vector origin;
    float radius;
    float height;

  public:
  	Cylinder()
  		: origin(Vector()), radius(1), height(2)  //Default constructor creates a unit Cylinder
  	{
  		color = Color::WHITE;
  	};

      Cylinder(Vector c, float r, float height, Color col)
  		: origin(c), radius(r), height(height)
  	{
  		color = col;
  	};

  	float intersect(Vector pos, Vector dir);

  	Vector normal(Vector p);

  };

  #endif //!H_CYLINDER
