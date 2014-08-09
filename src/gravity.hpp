#ifndef _GRAVITY_H
#define _GRAVITY_H

#include <vector>
#include "util.hpp"

class WorldObject;

/// Object that causes a gravitational force, helper class for GravityManager
class GravObject
{
	public:
		GravObject(SimpleVec3d pos, double mass) :
			pos(pos), mass(mass) {};
		SimpleVec3d pos;
		double mass;
}; // object that causes a gravitational force

/// Allows WorldObjects to retrieve their acceleration due to gravity
class GravityManager
{
	public:
		GravityManager(std::vector<WorldObject*>* objects);
		~GravityManager();
		SimpleVec3d getGravityAcc(SimpleVec3d  pos);

	private:
		std::vector<GravObject> m_objects;
};

#endif
