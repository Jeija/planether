#include "gllibs.h"
#include <string>

#include "util.h"

#ifndef _NAVIGATION_H
#define _NAVIGATION_H

class TeleportTarget;

/*
	This class provides support for a simple Navigation system that is supposed to aid the
	player in finding their destination
*/

/// Helps the player to navigate with the spaceship
class Navigator
{
	public:
		Navigator();
		~Navigator();

		void step(SimpleVec3d start_pos, SimpleVec3d translation);
		void renderPath(glm::quat spaceship_quat);

		void start();
		void stop();

		glm::quat getTargetQuat();

		void setTarget(TeleportTarget *target);

		bool getActive()
			{ return m_active; }

	private:
		SimpleVec3d m_target_pos;
		SimpleVec3d m_start_pos;
		std::string m_target_name;
		TeleportTarget *m_target;

		GLfloat m_vertices[2*3];
		GLubyte m_indices[2];

		bool m_active;
		bool m_ready; // vertices + indices created
};

#endif
