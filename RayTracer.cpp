// ========================================================================
// COSC 363  Computer Graphics  Lab07
// A simple ray tracer
// ========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include "Vector.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Plane.h"
#include "Color.h"
#include "Object.h"
#include "TextureBMP.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int PPU = 30;     //Total 600x600 pixels
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float PI = 3.14159265359;
const float DIAMOND = 2.4;
const float AIR = 1;

vector<Object*> sceneObjects;

Vector light;
Color backgroundCol;
TextureBMP earth;
TextureBMP checker;

//A useful struct
struct PointBundle
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
  Vector  point(0, 0, 0);
	float min = 10000.0;

	PointBundle out = {point, -1, 0.0};

  for(unsigned int i = 0;  i < sceneObjects.size();  i++)
	{
    float t = sceneObjects[i]->intersect(pos, dir);
		if(t > 0)        //Intersects the object
		{
			point = pos + dir*t;
			if(t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}

/*
* Computes the colour value obtained by tracing a ray.
* If reflections and refractions are to be included, then secondary rays will
* have to be traced from the point, by converting this method to a recursive
* procedure.
*/

Color trace(Vector pos, Vector dir, int step)
{
	Color colorSum;
  PointBundle q = closestPt(pos, dir);

  Vector lightVector = light - q.point;
	float lightDist = lightVector.length();
	lightVector.normalise();

	PointBundle s = closestPt(q.point, lightVector);
  if(q.index == -1) return backgroundCol;        //no intersection

	Color col = sceneObjects[q.index]->getColor(); //Object's colour

	Vector n = sceneObjects[q.index]->normal(q.point);
	Vector l = light - q.point;
	Vector v(-dir.x, -dir.y, -dir.z); //View vector;


	l.normalise();
	float lDotn = l.dot(n);

	if (q.index == 2 && step < MAX_STEPS){
		float u = asin(n.x) / PI + 0.5;
		float v = asin(n.y) / PI + 0.5;
		col = earth.getColorAt(u,v);
	}

	if (q.index == 3 && step < MAX_STEPS) {

		float u = (int) ((q.point.x - 10) / 3) % 2;
		float v = (int) ((q.point.z + 20) / 3) % 2;
		if ((u && v) || (!u && !v)) {
			col = Color::WHITE;
		} else {
			col = Color::BLACK;
		}
	}

	//////////////////// BROKEN REFRACTION ////////////////////////////////////
	////////////// UNCOMMENT IF YOU LIKE BROKEN STUFF /////////////////////////

	// if (q.index == 4 && step < MAX_STEPS) {
	// 	Vector entryPt = q.point;
	//
	// 	float cosThetaT1 = sqrt(1 - (pow(AIR / DIAMOND, 2) * (1 - pow(dir.dot(n), 2)) ));
	// 	Vector g1Dir = (dir * (AIR/DIAMOND)) - n * ((AIR/DIAMOND) * dir.dot(n) + cosThetaT1);
	//
	// 	Vector exitPt = closestPt(entryPt, g1Dir).point;
	// 	Vector n2 = sceneObjects[q.index]->normal(exitPt);
	//
	// 	float cosThetaT2 = sqrt(1 - (pow(DIAMOND / AIR, 2) * (1 - pow(g1Dir.dot(n2), 2)) ));
	// 	Vector g2Dir = (g1Dir * (DIAMOND / AIR)) - n2 * ((AIR/DIAMOND) * dir.dot(n2) + cosThetaT2);
	// 	col = trace(exitPt, g2Dir, step + 1);
	// }



	if (lDotn <= 0) {
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
	} else if (s.index>-1 && s.dist < lightDist) {
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
	} else {
		Vector r = ((n * 2) * lDotn) - l; // r = 2(L.n)n – L
		r.normalise();


		float rDotv = r.dot(v);
		float spec;
		if(rDotv < 0) {
			spec = 0.0;
		} else {
			spec = pow(rDotv, 5); //Phong exponent = 10
		}
		colorSum = col.phongLight(backgroundCol, lDotn, spec);
	}

	if (q.index == 0 && step < MAX_STEPS) {
		float nDotV = n.dot(v);
		Vector reflectionVector = ((n*2) * (nDotV)) - v;
		float reflCoeff = 0.4;
		Color reflectionCol = trace(q.point, reflectionVector, step+1);
		colorSum.combineColor(reflectionCol, reflCoeff);
	}
	return colorSum;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each pixel as quads.
//---------------------------------------------------------------------------------------
void display()
{
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0/PPU;
	float halfPixelSize = pixelSize/2.0;
	float x1, y1, xc, yc;
	Vector eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);  //Each pixel is a quad.

	for(int i = 0; i < widthInPixels; i++)	//Scan every "pixel"
	{
		x1 = XMIN + i*pixelSize;
		xc = x1 + halfPixelSize;
		for(int j = 0; j < heightInPixels; j++)
		{
			y1 = YMIN + j*pixelSize;
			yc = y1 + halfPixelSize;

		    Vector dir(xc, yc, -EDIST);	//direction of the primary ray

		    dir.normalise();			//Normalise this direction

		    Color col = trace (eye, dir, 1); //Trace the primary ray and get the colour value
				glColor3f(col.r, col.g, col.b);
				glVertex2f(x1, y1);				//Draw each pixel with its color value
				glVertex2f(x1 + pixelSize, y1);
				glVertex2f(x1 + pixelSize, y1 + pixelSize);
				glVertex2f(x1, y1 + pixelSize);
        }
    }

    glEnd();
    glFlush();
}



void initialize()
{
	//Iniitialize background colour and light's position
	backgroundCol = Color::GRAY;
	light = Vector(-30.0, 50.0, -5.0);

	earth = TextureBMP((char*)"Earth.bmp");


	Sphere *sphere1 = new Sphere(Vector(-5, 6, -50), 2.0, Color::RED);
	Sphere *sphere2 = new Sphere(Vector(0, 0, -55), 6.0, Color::GRAY);
	Sphere *sphere3 = new Sphere(Vector(5, 4, -45), 2.75, Color::WHITE);

	Cylinder *cyl = new Cylinder(Vector(7,-10,-45), 2, 5, Color::GREEN);

	Sphere *sphere4 = new Sphere(Vector(7,-3,-45), 3, Color::BLUE);

	Plane *plane = new Plane(Vector(-40, -10, -40), Vector(40, -10, -40),
		Vector(40., -10, -120), Vector(-40., -10, -120), Color::WHITE);


	/*
	A Lazy cuboid
  Bottom, left and back faces can be removed to improve performance, as they're not
	visible from the viewport or in the reflection
	*/

	Plane *cubeBottom = new Plane(Vector(-5, -10, -45), Vector(0., -10, -45),
		Vector(0., -10, -55), Vector(-5., -10, -55), Color::RED);
	Plane *cubeLeft = new Plane(Vector(-5, -10, -55), Vector(-5, -10, -45),
	  Vector(-5, -7, -45), Vector(-5, -7, -55), Color::BLUE);
	Plane *cubeBack = new Plane(Vector(-5, -10, -55), Vector(-0, -10, -55),
		Vector(0, -7, -55), Vector(-5, -7, -55), Color::RED);
	Plane *cubeFront = new Plane(Vector(-5, -10, -45), Vector(-0, -10, -45),
		Vector(0, -7, -45), Vector(-5, -7, -45), Color::BLUE);
	Plane *cubeTop = new Plane(Vector(-5, -7, -45), Vector(0., -7, -45),
	Vector(0., -7, -55), Vector(-5., -7, -55), Color::BLUE);
	Plane *cubeRight = new Plane(Vector(0, -10, -55), Vector(0, -10, -45),
			Vector(0, -7, -45), Vector(0, -7, -55), Color::RED);

	sceneObjects.push_back(sphere2); // 0
	sceneObjects.push_back(sphere1); // 1
	sceneObjects.push_back(sphere3); // 2
	sceneObjects.push_back(plane);   // 3
	sceneObjects.push_back(sphere4); // 4
	sceneObjects.push_back(cyl);     // 5

	sceneObjects.push_back(cubeBottom);
	sceneObjects.push_back(cubeLeft);
	sceneObjects.push_back(cubeBack);
	sceneObjects.push_back(cubeFront);
	sceneObjects.push_back(cubeTop);
	sceneObjects.push_back(cubeRight);

	//The following are OpenGL functions used only for drawing the pixels
	//of the ray-traced scene.
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
