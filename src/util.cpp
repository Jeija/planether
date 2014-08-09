#include <iostream>
#include <sstream>
#include <math.h>
#include <string>

#include "quatutil.h"
#include "config.h"
#include "debug.h"
#include "util.h"

/*
	SimpleVec3d:
*/

SimpleVec3d::SimpleVec3d(SimpleAngles a)
{
	x = sin(a.yaw) * cos(a.pitch);
	y = sin(a.pitch);
	z = -cos(a.yaw) * cos(a.pitch);
	//x = 	  cos(a.pitch) * cos(a.yaw) * cos(a.pitch) * cos(a.yaw) - sin(a.pitch);
	//y = 	  sin(a.roll) * sin(a.pitch) * cos(a.yaw)
	//	- cos(a.roll) * sin(a.yaw  ) * sin(a.roll) * sin(a.pitch) * sin(a.yaw)
	//	+ cos(a.roll) * cos(a.yaw  ) * sin(a.roll) * cos(a.pitch);
	//z = 	  cos(a.roll) * sin(a.pitch) * cos(a.yaw)
	//	+ sin(a.roll) * sin(a.yaw  ) * cos(a.roll) * sin(a.pitch) * sin(a.yaw)
	//	- sin(a.roll) * cos(a.yaw  ) * cos(a.roll) * cos(a.pitch);
}

SimpleVec3d::SimpleVec3d(SphericalVector3f sv)
{
	x = sv.radius * sin(sv.inclination) * cos(sv.azimuth);
	y = sv.radius * sin(sv.inclination) * sin(sv.azimuth);
	z = sv.radius * cos(sv.inclination);
}

SimpleVec3d::SimpleVec3d(glm::vec3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

SimpleVec3d::SimpleVec3d(const SimpleVec3d &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

SimpleVec3d SimpleVec3d::normalize()
{
	double len = getVectorLength(*this);

	x /= len;
	y /= len;
	z /= len;

	return *this;
}

SimpleVec3d SimpleVec3d::rotateBy(SimpleVec3d axis, float angle)
{
	return SimpleVec3d(glm::rotate(toVec3(), angle, axis.toVec3()));
}

float* SimpleVec3d::toFloatArray()
{
	static float array[3];

	array[0] = x;
	array[1] = y;
	array[2] = z;

	return array;
}

/*
	SphericalVector:
*/
SphericalVector3f::SphericalVector3f(const SphericalVector3f &sv)
{
	azimuth = sv.azimuth;
	inclination = sv.inclination;
	radius = sv.radius;
}

/*
	SimpleAngles:
*/

SimpleAngles::SimpleAngles(SimpleVec3d v)
{
	roll = 0;
	yaw =	-atan2(v.x, -v.z);
	pitch =	 atan2(v.y, sqrt((v.x * v.x) + (v.z * v.z)));
}

/*
	SimpleColor:
	OpenGL-Specific part:
*/

void SimpleColor::setAmbient ()
{
	float color[] = {r, g, b, a};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
}

void SimpleColor::setDiffuse ()
{
	float color[] = {r, g, b, a};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
}

void SimpleColor::setSpecular()
{
	float color[] = {r, g, b, a};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
}


void SimpleColor::setEmission()
{
	float color[] = {r, g, b, a};
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
}

/*
	Other helper utilities:
*/

SimpleVec3d crossProduct(SimpleVec3d a, SimpleVec3d b)
{
	SimpleVec3d product = SimpleVec3d (
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);

	return product;
}


// Create an arbitrary vector that is perpendicular to vec 
SimpleVec3d getVectorPerpendicular(SimpleVec3d vec)
{
	SimpleVec3d cr(1.0, 0.0, 0.0);
	if (cr == vec)
		cr = SimpleVec3d(0.0, 1.0, 0.0);

	// Calculate the crossProduct of vec with any vector that is not the same as vec
	return crossProduct(vec, cr);
}

double dotProduct(SimpleVec3d a, SimpleVec3d b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double angleBetween (SimpleVec3d a, SimpleVec3d b)
{
	return acos(dotProduct(a, b) / (getVectorLength(a) * getVectorLength(b)));
}

double getVectorLength(SimpleVec3d vec)
{
	return (sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z));
}

double getAngleDifference(double x, double y)
{
	return atan2(sin(x-y), cos(x-y));
}

void resetMaterial()
{
	// Default values:
	float ambient[]  = {0.2, 0.2, 0.2, 1.0};
	float diffuse[]  = {0.8, 0.8, 0.8, 1.0};
	float specular[] = {0.0, 0.0, 0.0, 1.0};
	float emission[] = {0.0, 0.0, 0.0, 1.0};
	float shininess  = 0;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , ambient  );
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , diffuse  );
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , specular );
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , emission );
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glColor4f(1, 1, 1, 1);
}

std::string texPath(std::string texture)
{
	return (getBasedir() + std::string("textures") + DIR_DELIM + texture);
}

void randomPlanetPosition(SimpleVec3d *pos, SimpleVec3d *vel, SimpleVec3d gravcen, SimpleVec3d axis)
{
	// Generate random rotation by the given parameters
	double randangle = 1. * rand() / RAND_MAX * 2 * PI;
	glm::quat randquat = glm::angleAxis((float)randangle, axis.normalize().toVec3());

	// Rotate velocity of the Planet
	double vel_len = getVectorLength(*vel);
	*vel = rotateVecByQuat((*vel).normalize(), randquat).normalize();
	(*vel) *= vel_len;

	// Rotate the relative position of the planet around the gravity center
	// (sun / star / other planet in case this "Planet" is actually a moon)
	SimpleVec3d pos_rel = *pos - gravcen;

	double pos_len = getVectorLength(pos_rel);
	pos_rel = rotateVecByQuat(pos_rel, randquat).normalize();
	pos_rel *= pos_len;

	*pos = gravcen + pos_rel;
}

#define MAX_PATHLEN 1024

// Run-in-place edition for Unixes (Linux / FreeBSD)
#ifndef PLANETHER_WINDOWS // Linux
#include <unistd.h>
#include <cstring>


std::string getBasedir()
{
	char buf[MAX_PATHLEN];
	ssize_t len = readlink("/proc/self/exe", buf, MAX_PATHLEN-1);
	if (len == -1)
		len = readlink("/proc/curproc/file", buf, MAX_PATHLEN-1);
	buf[len] = 0x00;

	std::string path = std::string(buf);			// now in BASE/bin/planether
	path = path.substr(0, path.find_last_of(DIR_DELIM));	// now in BASE/bin
	path = path.substr(0, path.find_last_of(DIR_DELIM));	// now in BASE
	
	return path + DIR_DELIM;
}
#else // Windows run-in-place

// For now, just assume the working directory is OK
std::string getBasedir()
{
	return std::string();
}

#endif
