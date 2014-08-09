#ifndef UTIL_H
#define UTIL_H

#define PI 3.14159265358979323846
#define RADTODEG(x) ((x) * 57.29578)

#include <iostream>
#include <math.h>
#include "gllibs.hpp"

class SimpleAngles;
class SphericalVector3f;
class SimpleVec3d;

/// A 3-Dimensional double vector
class SimpleVec3d
{
	public:
		SimpleVec3d() : x(0), y(0), z(0) {};
		SimpleVec3d(double x, double y, double z) : x(x), y(y), z(z) {};
		SimpleVec3d(SimpleAngles a);
		SimpleVec3d(SphericalVector3f sv);
		SimpleVec3d(glm::vec3 v);
		SimpleVec3d(const SimpleVec3d &vec); // copy constructor

		SimpleVec3d normalize();
		SimpleVec3d rotateBy(SimpleVec3d axis, float angle);
		float *toFloatArray();

		double x;
		double y;
		double z;

		SimpleVec3d operator + (const SimpleVec3d s)
			{ return SimpleVec3d(x + s.x, y + s.y, z + s.z); };

		SimpleVec3d operator - (const SimpleVec3d s)
			{ return SimpleVec3d(x - s.x, y - s.y, z - s.z); };

		SimpleVec3d operator += (const SimpleVec3d s)
			{ x += s.x; y += s.y; z += s.z; return *this; };

		SimpleVec3d operator -= (const SimpleVec3d s)
			{ x -= s.x; y -= s.y; z -= s.z; return *this; };

		SimpleVec3d operator *= (double m)
			{ x *= m; y *= m; z *= m; return *this; };

		SimpleVec3d operator * (double m)
			{ return SimpleVec3d(x * m, y * m, z * m); };

		SimpleVec3d operator / (double m)
			{ return SimpleVec3d(x / m, y / m, z / m); };

		void operator = (const SimpleVec3d v)
			{ x = v.x; y = v.y; z = v.z; };

		bool operator == (const SimpleVec3d v)
			{ return ((x == v.x) && (y == v.y && (z == v.z))); }

		// OpenGL-Specific:
		void translate ()
			{ glTranslated(x, y, z); };

		void vertex ()
			{ glVertex3d(x, y, z); };

		void normal ()
			{ glNormal3d(x, y, z); };

		// GLM-Specific
		glm::vec3 toVec3()
			{ return glm::vec3(x, y, z); };
};

/*
	Spherical Coordinates
	inclination is the same axis like pitch
	azimuth can be compared to yaw
*/
/// A vector for a 3-Dimensional polar coordinate system
class SphericalVector3f
{
	public:
		SphericalVector3f() : radius(0), inclination(0), azimuth(0) {};
		SphericalVector3f(float r, float i, float a) : radius(r), inclination(i), azimuth(a) {};
		SphericalVector3f(const SphericalVector3f &sv);

		float radius;
		float inclination;
		float azimuth;

		SphericalVector3f operator + (const SphericalVector3f s)
		{
			SphericalVector3f newvec(	radius		+ s.radius,
							inclination	+ s.inclination,
							azimuth		+ s.azimuth);
			newvec.inclination	= fmod(newvec.inclination, 	PI*2);
			newvec.azimuth		= fmod(newvec.azimuth,		PI);
			return newvec;
		};
};

/// The three euler angles
class SimpleAngles
{
	public:
		SimpleAngles() : yaw(0), pitch(0), roll(0) {};
		SimpleAngles(double yaw, double pitch, double roll) :
			yaw(yaw), pitch(pitch), roll(roll) {};
		SimpleAngles(SimpleVec3d v);

		double yaw;
		double pitch;
		double roll;

		SimpleAngles operator + (const SimpleAngles s)
			{ return SimpleAngles(yaw + s.yaw, pitch + s.pitch, roll + s.roll); };

		SimpleAngles operator += (const SimpleAngles s)
			{ yaw += s.yaw; pitch += s.pitch; roll += s.roll; return *this; };

		SimpleAngles operator * (double m)
			{ return SimpleAngles(yaw * m, pitch * m, roll * m); };

		SimpleAngles operator - (const SimpleAngles s)
			{ return SimpleAngles(yaw - s.yaw, pitch - s.pitch, roll - s.roll); };
};

/// A color reference that can be used for lighting etc.
class SimpleColor
{
	public:
		SimpleColor() : r(0), g(0), b(0), a(1) {};
		SimpleColor(float r, float g, float b) : r(r), g(g), b(b), a(1) {};
		SimpleColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};

		void set()
			{ glColor4f(r, g, b, a); }
		void setNonAlpha()
			{ glColor3f(r, g, b); }

		float r, g, b, a;

		// OpenGL-Specific:
		void setAmbient ();
		void setDiffuse ();
		void setSpecular();
		void setEmission();
};

SimpleVec3d	crossProduct		(SimpleVec3d a, SimpleVec3d b);
double		dotProduct		(SimpleVec3d a, SimpleVec3d b);
SimpleVec3d	getVectorPerpendicular	(SimpleVec3d vec);
double		getVectorLength 	(SimpleVec3d vec);

// Get angle between two vectors (in radians)
double angleBetween (SimpleVec3d a, SimpleVec3d b);

// Get angle difference between to angles (in radians), also considers
// the shortest path between the two
double getAngleDifference(double a, double b);

// Reset all glMaterialf/v calls
void resetMaterial();

// Get the full path to a texture when just having the texture's name (textures folder)
std::string texPath(std::string texture);

// Rotate position and velocity of a Planet around a gravity center (e.g. sun) around the given axis
void randomPlanetPosition(SimpleVec3d *pos, SimpleVec3d *vel, SimpleVec3d gravcen, SimpleVec3d axis);

// Find out if a and b only diverge by maxdiff at maximum
inline bool almostEqual(float a, float b, float maxdiff)
{
	return (a + maxdiff > b && b + maxdiff > a);
}

// Get the current basedir path that the textures / shaders / sounds folders should be in
std::string getBasedir();

#endif
