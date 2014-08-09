#include <map>

#include "objects.h"
#include "gllibs.h"
#include "util.h"

#ifndef PLAYER_H
#define PLAYER_H

/// Contains information about the position, look direction, movement etc. of the Camera and takes input
class Player : public PhysicalObject
{
	public:
		Player(SimpleVec3d pos, SimpleAngles angles);
		~Player();

		void rotView(float xmove, float ymove);
		void addRotSpeed(float xmov, float ymov);

		SimpleAngles getAngles();
		void setLookAngles(SimpleAngles look);

		SimpleVec3d getLookAxis()	// Look direction of the player
			{ return m_lookaxis; };
		SimpleVec3d getUpAxis()	// Upaxis of the player
			{ return m_upaxis; };
		SimpleVec3d getRightAxis()	// Axis to the right of the player
			{ return m_rightaxis; };
		glm::quat getLookQuat()
			{ return m_look; }
		void setLookQuat(glm::quat look);
		void addLookQuat(glm::quat addquat)
			{ m_look *= addquat; }

		void rotRoll(float r);
		void accRollSpeed(float r);
		void setRollSpeed(float r);

		SimpleVec3d getPos()
			{ return m_pos; }
		void setPos(SimpleVec3d pos)
			{ m_pos = pos; }
		void moveBy(SimpleVec3d by)
			{ m_pos += by; }
		void moveBy(double x, double y, double z)
			{ m_pos += SimpleVec3d(x, y, z); }

		/* Velocity */
		void setVelocity(SimpleVec3d velocity)
			{ m_velocity = velocity; }
		void addVelocity(SimpleVec3d velocity)
			{ m_velocity += velocity; }
		SimpleVec3d getVelocity ()
			{ return m_velocity; }

		void render() {}; // The Player is not rendered
		void step(float dtime);

		// KeyBoard
		void onKeyAction(unsigned char key, bool pressed);

		// Mouse
		void onMouseClick(int button, int state, int x, int y);
		void onMouseMove(int x, int y);
		void onKeyCallback(bool keystate[255], bool ks_special[255], float dtime);

	private:
		// Wrappers
		static void onKeyPress_wrapper(unsigned char key, void *self);
		static void onKeyRelease_wrapper(unsigned char key, void *self);
		static void onMouseMove_wrapper(int x, int y, void *self);
		static void onMouseClick_wrapper(int button, int state, int x, int y, void *self);
		static void onKeyCallback_wrapper(bool keystate[255], bool ks_special[255],
			float dtime, void *self);

		glm::quat m_look;
		glm::quat m_rotvel;

		// Mouse rotation to apply when stepping
		glm::quat m_rot_todo;

		SimpleVec3d m_lookaxis;
		SimpleVec3d m_upaxis;
		SimpleVec3d m_rightaxis;

		// Velocity produced by keyboard / mouse, always relative to look direction
		SimpleVec3d m_movevel;
		SimpleVec3d m_movevel_toremove; // move velocity added last step, to be removed later

		// Player keeps track of the keypresses on its own to not get disturbed when
		// the TeleportWindow is open, a key is pressed and only released when the window
		// is closed again
		std::map <int, bool> m_keystate;

		// Saves state of mouse buttons; at the beginning assume that no button is pressed
		// Used to only perform an action if this state changes
		// GLUT_DOWN = pressed; GLUT_UP = not pressed
		// Assume there is no mouse with more than 20 buttons, but will be checked in code
		// again in order not to produce any buffer overflow.
		std::map <int, int> m_mouse;
};

#endif
