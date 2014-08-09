#include "navigation.hpp"
#include "quatutil.hpp"
#include "objects.hpp"
#include "audio.hpp"
#include "util.hpp"


#ifndef _SPACESHIP_H
#define _SPACESHIP_H

class Player;
class Navigator;
class FireParticleSource;

enum CamBindType
{
	CAMERA_BOUND,
	CAMERA_RELATIVE_ROT,
	CAMERA_RELATIVE,
	CAMERA_FREE
};

/// The SpaceShip the player can use to travel in the solar system
class SpaceShip : public PhysicalObject, public AudioObject
{
	public:
		SpaceShip(SimpleVec3d pos, SimpleVec3d velocity, glm::quat quat,
			glm::quat velquat);
		~SpaceShip();

		void render();

		// Movement in main thread, before all the particles
		void stepMainThread(float dtime);
		void step(float dtime);
		void stepAudio();

		SimpleVec3d getVelocity()
			{ return m_velocity; };
		void setVelocity(SimpleVec3d velocity)
			{ m_velocity = velocity; }

		void processKeys(bool keystate[255], bool keystate_special[255], float dtime);
		void bindCamera(float dtime);

		CamBindType getCamBind()
			{ return m_cambound; }
		SimpleVec3d getGravityAcc()
			{ return m_gravity_acc; }
		void setPos(SimpleVec3d pos)
			{ m_pos = pos; }
		void setAngles(SimpleAngles angles)
			{ m_quat = anglesToQuat(angles); }
		void setVelQuat(glm::quat velquat)
			{ m_velquat = velquat; }
		SimpleVec3d getPos()
			{ return m_pos; }
		Navigator *getNavigator()
			{ return m_navigator; }

	private:
		float m_time;
		FireParticleSource *m_psource;

		glm::quat m_quat; // saves orientation in space
		glm::quat m_velquat; // saves rotational velocity
		glm::quat m_accquat; // saves rotational acceleration

		static void process_keys_wrapper(bool keystate[255], bool keystate_special[255],
			float dtime, void *self);
		void setAcc(float yacc, float pacc, float racc);
		CamBindType m_cambound;
		static void onKeyPress_wrapper(unsigned char key, void *self);
		void onKeyPress(unsigned char key);
		SimpleVec3d m_engine_acc;
		std::vector <SimpleVec3d>m_predicted_route;
		float m_time_since_acc; // time since last accelerated, used for particle animation
		bool m_engine_running;

		// Used for binding the camera:
		SimpleVec3d m_relpos_old;
		glm::quat m_quat_old;

		SimpleVec3d m_gravity_acc;

		AudioNode m_audio;

		Navigator *m_navigator;

		// Velocity last added to the player, must be removed next time again
		SimpleVec3d m_player_lastadd_vel;
};

#endif
