// Quaternion utilities
// partly taken from http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions
// especially the RotationBetweenVectors function is copied and adapted to Planethers needs
// e.g. make use of SimpleVec3d and crossProduct, dotProduct etc.

#include "quatutil.hpp"
#include "debug.hpp"
#include "util.hpp"

glm::quat RotationBetweenVectors(SimpleVec3d start, SimpleVec3d dest)
{
	start.normalize();
	dest.normalize ();

	float cosTheta = dotProduct(start, dest);
	SimpleVec3d rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		// special case when vectors in opposite directions:
		return glm::angleAxis((float)PI, getVectorPerpendicular(start).toVec3());
	}

	rotationAxis = crossProduct(start, dest);
 
	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::normalize(glm::quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	));
}

glm::quat conjugateQuat(glm::quat q)
{
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	return glm::normalize(q);
}

glm::quat toQuaternion(SimpleVec3d vec)
{
	glm::quat q;
	q.x = vec.x;
	q.y = vec.y;
	q.z = vec.z;
	q.w = 0;

	return q;
}

void quatToMounting(glm::quat rotquat, SimpleVec3d *look, SimpleVec3d *up, SimpleVec3d *right)
{
	// Look Vector
	SimpleVec3d defaultLook(0, 0, -1);
	*look = rotateVecByQuat(defaultLook, rotquat);

	// Right Vector
	SimpleVec3d defRightAxis(1, 0, 0);
	*right = rotateVecByQuat(defRightAxis, rotquat);

	// Up Vector
	*up = crossProduct(*right, *look);
}

glm::quat anglesToQuat(SimpleAngles ang)
{
	SimpleVec3d defLookDir(0, 0, -1);
	return glm::normalize(RotationBetweenVectors(defLookDir, SimpleVec3d(ang)));
}

SimpleAngles quatToAngles(glm::quat rotquat)
{
	SimpleVec3d defaultLook(0, 0, -1);
	glm::quat deflookquat = toQuaternion(defaultLook);
	glm::quat lookquat = rotquat * deflookquat * conjugateQuat(rotquat);

	return SimpleAngles(SimpleVec3d(lookquat.x, lookquat.y, lookquat.z));
}

SimpleVec3d quatToVector(glm::quat rotquat)
{
	SimpleVec3d defaultLook(0, 0, -1);
	glm::quat deflookquat = toQuaternion(defaultLook);
	glm::quat lookquat = rotquat * deflookquat * conjugateQuat(rotquat);

	return SimpleVec3d(lookquat.x, lookquat.y, lookquat.z);
}

SimpleVec3d rotateVecByQuat(SimpleVec3d vec, glm::quat quat)
{
	glm::quat vecquat = toQuaternion(vec);
	glm::quat result = quat * vecquat * conjugateQuat(quat);

	return SimpleVec3d(result.x, result.y, result.z);
}
