#include <iostream>
#include "util.h"

void dump_vector(SimpleVec3d vec)
{
	std::cout<<"Vector: x="<<vec.x<<" y="<<vec.y<<" z="<<vec.z<<std::endl;
}

void dump_angles(SimpleAngles ang)
{
	std::cout<<"Vector: yaw="<<ang.yaw<<" pitch="<<ang.pitch<<" roll="<<ang.roll<<std::endl;
}
