#include "objects.h"

/**
 * Moves the object by applying the laws of physics. Moves the object by its velocity and accelerates
 * the velocity by the objects acceleration.
 */
void PhysicalObject::physicalMove(float dtime)
{
	m_pos += m_velocity * dtime;
	m_velocity += m_acceleration * dtime;
}
