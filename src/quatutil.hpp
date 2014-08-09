// Quaternion utilities
// partly taken from http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions

#include "gllibs.hpp"
#include "util.hpp"

#ifndef _QUATUTIL_H
#define _QUATUTIL_H

glm::quat RotationBetweenVectors(SimpleVec3d start, SimpleVec3d dest);
glm::quat conjugateQuat(glm::quat q);
glm::quat toQuaternion(SimpleVec3d vec);
void quatToMounting(glm::quat rotquat, SimpleVec3d *look, SimpleVec3d *up, SimpleVec3d *right);
glm::quat anglesToQuat(SimpleAngles ang);
SimpleAngles quatToAngles(glm::quat rotquat);
SimpleVec3d quatToVector(glm::quat rotquat);
SimpleVec3d rotateVecByQuat(SimpleVec3d vec, glm::quat quat);

#endif
