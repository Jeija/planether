#include "gamevars.h"
#include "gravity.h"
#include "objects.h"
#include "config.h"
#include "debug.h"
#include "util.h"

/**
 * \brief Create a new GravityManager for objects
 * \param objects All objects in the WorldEnvironment
 */
GravityManager::GravityManager(std::vector<WorldObject*>* objects)
{
	// Copy environment so that the order in which planetary movement is called won't matter
	// as we always use the inital environment for acceleration calculations
	for (auto obj : *objects)
	{
		MassObject *obj_m = dynamic_cast<MassObject *>(obj);
		if (obj_m) // only if object is a MassObject
			m_objects.push_back(GravObject(obj->getPos(), obj_m->getMass()));
	}
}

GravityManager::~GravityManager()
{
	m_objects.clear();
}

/**
 * \brief Get the acceleration caused by gravity at the given position
 * \param pos The Position to calculated the gravity acceleration for
 *
 * If there is an object right at pos, it will be ignored
 */
SimpleVec3d GravityManager::getGravityAcc(SimpleVec3d pos)
{
	SimpleVec3d gravityacc(0, 0, 0);

	for (auto obj : m_objects)
	{
		SimpleVec3d diffvec =
			(obj.pos/USC - pos/USC);
		double distance = getVectorLength(diffvec) * 1000.; // *1000 -> in meters
		if (obj.mass != 0 && distance != 0)
		{
			SimpleVec3d dir = diffvec.normalize();
			gravityacc += dir * (obj.mass / (distance * distance)) * GRAV_CONST / 1000.;
			// / 1000 --> from meters back to km
		}
	}

	return gravityacc * USC;
}
