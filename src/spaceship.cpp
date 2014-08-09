#include <tgmath.h>
#include <math.h>

#include "environment.hpp"
#include "spaceship.hpp"
#include "particle.hpp"
#include "drawutil.hpp"
#include "keyboard.hpp"
#include "gamevars.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "gllibs.hpp"
#include "player.hpp"
#include "debug.hpp"
#include "game.hpp"
#include "util.hpp"


// 200km x 100km x 500km
#define SPACESHIP_X (  50   /2.) * USC
#define SPACESHIP_Y (  25   /2.) * USC
#define SPACESHIP_Z ( 125   /2.) * USC
#define PARTICLE_MAXINTENSITY 1000
#define PARTICLE_FLOWDURATION 0.2 	// duration in seconds for particles to be emitted after no
					// acceleration occured
#define DETAIL 40 // vertices on cylinders / spheres

/*
	SpaceShip
*/

SpaceShip::SpaceShip(SimpleVec3d pos, SimpleVec3d velocity, glm::quat quat, glm::quat velquat) :
m_time(0),
m_quat(quat),
m_velquat(velquat),
m_cambound(CAMERA_BOUND),
m_time_since_acc(PARTICLE_FLOWDURATION),
m_engine_running(false)
{
	// AudioNode
	m_audio.setGain(4.0);

	// PhysicalObject
	m_pos = pos;
	m_velocity = velocity;

	// Keyboard Callbacks
	keyboard->registerCallback(process_keys_wrapper, this);
	keyboard->registerKeyPressCallback(onKeyPress_wrapper, this);

	// FireParticleSource
	m_psource = new FireParticleSource(m_pos + SimpleVec3d(0, 0, SPACESHIP_Z),
					SimpleVec3d(0, 0, -USC*1000), PI / 20,
					SPACESHIP_Y / 10, SPACESHIP_Y / 2, 300*USC, 800*USC, 0.2, 0.8,
					"spaceshipfire");
	game->getWorldEnv()->addObject(m_psource);

	// Navigator
	m_navigator = new Navigator();
}

SpaceShip::~SpaceShip()
{
	std::cout<<"~SpaceShip"<<std::endl;

	//delete m_psource; is a WorldObject, will be deleted by WorldEnvironment
	delete m_navigator;
}

void SpaceShip::render ()
{
	// Predicted Route
	glColor4f(1.0, 0.0, 0.0, 1.0);
	SimpleColor(1, 0, 0, 1).setEmission();
	glLineWidth(1.0f);
	glBegin(GL_LINE_STRIP);
	{
		for (auto v : m_predicted_route)
			v.vertex();
	}
	glEnd();
	SimpleColor(0, 0, 0, 1).setEmission(); // reset emission

	// Navigator
	m_navigator->renderPath(m_quat);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glm::mat4 dirmat = glm::toMat4(m_quat);
	glMultMatrixf(glm::value_ptr(dirmat));

	glRotated(180, 0, 1, 0);

	game->getCamera()->getShaderManager()->requestShader("spaceship");

	// Main body:
	SimpleVec3d p1(-SPACESHIP_X, -SPACESHIP_Y, -SPACESHIP_Z);
	SimpleVec3d p2( SPACESHIP_X,  SPACESHIP_Y,  SPACESHIP_Z);
	makeCuboid(p1, p2, 3, 3, 3);

	// Backside
	glPushMatrix();
	{
		glTranslatef(-SPACESHIP_X, 0, -SPACESHIP_Z);
		glutSolidSphere(SPACESHIP_Y, DETAIL, DETAIL);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(SPACESHIP_X, 0, -SPACESHIP_Z);
		glutSolidSphere(SPACESHIP_Y, DETAIL, DETAIL);
	}
	glPopMatrix();


	// Left / Right
	glPushMatrix();
	{
		glTranslatef(SPACESHIP_X, 0, -SPACESHIP_Z);
		makeCylinder(SPACESHIP_Y, SPACESHIP_Z*2, DETAIL);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-SPACESHIP_X, 0, -SPACESHIP_Z);
		makeCylinder(SPACESHIP_Y, SPACESHIP_Z*2, DETAIL);
	}
	glPopMatrix();


	// Front
	glPushMatrix();
	{
		glColor4f(0, 0.1, 0.4, 0.5);
		glTranslatef(-SPACESHIP_X, 0, SPACESHIP_Z);
		glRotatef(90, 0, 1, 0);
		GLUquadric* qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluCylinder(qobj, SPACESHIP_Y, SPACESHIP_Y, (SPACESHIP_X*2), DETAIL, DETAIL);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glColor4f(0, 0.1, 0.4, 0.5);
		glTranslatef(-SPACESHIP_X, 0, SPACESHIP_Z);
		glutSolidSphere(SPACESHIP_Y, DETAIL, DETAIL);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glColor4f(0, 0.1, 0.4, 0.5);
		glTranslatef(SPACESHIP_X, 0, SPACESHIP_Z);
		glutSolidSphere(SPACESHIP_Y, 100, 100);
	}
	glPopMatrix();

	// Cockpit
	glPushMatrix();
	{
		glColor4f(0, 0.1, 0.4, 0.5);
		glScalef(SPACESHIP_Y, SPACESHIP_Y, SPACESHIP_Y);
		glutSolidDodecahedron();
	}
	glPopMatrix();
}

void SpaceShip::bindCamera(float dtime)
{
	game->getPlayer()->addVelocity(m_player_lastadd_vel * -1);
	m_player_lastadd_vel = SimpleVec3d();

	Player *player = game->getPlayer();
	if (m_cambound == CAMERA_BOUND)
	{
		glm::quat lookquat;
		double folspeed = dtime * SPACESHIP_FOLLOWSPEED;
		if (folspeed > 1) folspeed = 1;

		// Desired vector for player to move for next frame
		SimpleVec3d t_playerpos = m_pos - quatToVector(m_quat) * 500.0 * USC
			- game->getPlayer()->getPos();

		player->setVelocity(t_playerpos / dtime);

		lookquat = glm::slerp(player->getLookQuat(), m_quat, (float)(folspeed));

		player->setLookQuat(lookquat);
	}
	else if (m_cambound == CAMERA_RELATIVE)
	{
		game->getPlayer()->addVelocity(m_velocity);
		m_player_lastadd_vel = m_velocity;
	}
	else if (m_cambound == CAMERA_RELATIVE_ROT)
	{
		// Desired vector for player to move for next frame
		SimpleVec3d t_playerpos  = m_pos + rotateVecByQuat(m_relpos_old, m_quat)
			- game->getPlayer()->getPos();
		game->getPlayer()->addVelocity(t_playerpos / dtime);
		m_player_lastadd_vel = t_playerpos / dtime;
		player->setLookQuat(m_quat * conjugateQuat(m_quat_old) * player->getLookQuat());
	}
}

void SpaceShip::stepMainThread (float dtime)
{
	// Used for camera binding:
	m_relpos_old = rotateVecByQuat(game->getPlayer()->getPos() - m_pos, conjugateQuat(m_quat));
	m_quat_old = m_quat;

	m_gravity_acc = game->getWorldEnv()->getGravityAcc(m_pos);
	m_acceleration  = m_engine_acc + m_gravity_acc;

	physicalMove(dtime);
	m_time += dtime;
	m_time_since_acc += dtime;

	// SpaceShip uses a dtime that is 10 times faster
	// so that speeds of 10 rotations per second
	// can be achieved. Otherwise, quaternions-rotations would cancel out and therefore
	// go backward again
	float dtime_fast = dtime * 10.0;
	m_velquat	= glm::slerp(m_velquat, m_velquat * m_accquat, dtime_fast);
	m_quat		= glm::slerp(m_quat, m_quat  * m_velquat, dtime_fast);

	m_quat		= glm::normalize(m_quat);
	m_velquat	= glm::normalize(m_velquat);
	m_accquat	= glm::normalize(m_accquat);

	bindCamera(dtime);

	/*
		Move FireParticleSource
	*/
	float particleIntensity = 1.0 - (m_time_since_acc / PARTICLE_FLOWDURATION);
	if (particleIntensity < 0) particleIntensity = 0;
	m_psource->setIntensity(PARTICLE_MAXINTENSITY * particleIntensity);
	m_psource->setPos(m_pos - quatToVector(m_quat) * 50.0 * USC);
	m_psource->setDir(quatToVector(m_quat) * -1);
	m_psource->setInitialVelocity(m_velocity);
}

void SpaceShip::step (float dtime)
{
	/*
		Predict route
	*/
	m_predicted_route.clear();
	m_predicted_route.push_back(SimpleVec3d());

	SimpleVec3d fut_pos;
	SimpleVec3d fut_vel = m_velocity;
	for (uint32_t i = 0; i <= 10000; i+=20) // TODO: Simulation Environment
	{
		fut_vel += game->getWorldEnv()->getGravityAcc(fut_pos + m_pos);
		fut_pos += fut_vel;
		m_predicted_route.push_back(fut_pos);
	}

	// Navigator
	m_navigator->step(m_pos, m_pos);
}

void SpaceShip::stepAudio()
{
	if (m_engine_running && m_audio.getFinished())
	{
		m_audio.setLoop(AL_TRUE);
		m_audio.addFile("spaceship_run.ogg");
		m_audio.play();
	}
	m_audio.updatePos(m_pos, m_velocity);
}

void SpaceShip::process_keys_wrapper(bool keystate[255], bool keystate_special[255],
	float dtime, void *self)
{
	((SpaceShip *)self)->processKeys(keystate, keystate_special, dtime);
}

void SpaceShip::onKeyPress_wrapper (unsigned char key, void *self)
{
	((SpaceShip *)self)->onKeyPress(key);
}

void SpaceShip::onKeyPress (unsigned char key)
{
	if (game->getTportOverlay()) return;

	if (key == (unsigned char)'v')
	{
		game->getPlayer()->setVelocity(SimpleVec3d());
		switch (m_cambound)
		{
			case CAMERA_BOUND:
				m_cambound = CAMERA_RELATIVE_ROT;
				break;
			case CAMERA_RELATIVE_ROT:
				m_cambound = CAMERA_RELATIVE;
				break;
			case CAMERA_RELATIVE:
				m_cambound = CAMERA_FREE;
				break;
			case CAMERA_FREE:
				m_cambound = CAMERA_BOUND;
				break;
		}
		game->getPlayer()->setVelocity(SimpleVec3d());
		m_player_lastadd_vel = SimpleVec3d();
	}
}

void SpaceShip::setAcc(float yacc, float pacc, float roll)
{
	m_accquat = anglesToQuat(SimpleAngles(yacc, pacc, 0));

	// Create the Quaternion from upvector and upvector with roll
	glm::quat rollquat = RotationBetweenVectors(
		SimpleVec3d(0, 1, 0),
		SimpleVec3d(SimpleAngles(PI/2, PI/2 - roll, 0))
	);

	// Apply rotation Quaternion (rollquat) to look Quaternion
	m_accquat *= rollquat;
}

void SpaceShip::processKeys(bool keystate[255], bool keystate_special[255], float dtime)
{
	float yacc = 0;
	float pacc = 0;
	float racc = 0;

	// Special Keys:
	if (keystate_special[GLUT_KEY_DOWN])
		pacc += SPACESHIP_ROTACC;

	if (keystate_special[GLUT_KEY_UP])
		pacc -= SPACESHIP_ROTACC;

	if (keystate_special[GLUT_KEY_LEFT])
		yacc -= SPACESHIP_ROTACC;

	if (keystate_special[GLUT_KEY_RIGHT])
		yacc += SPACESHIP_ROTACC;

	if (keystate[(uint8_t)'x'])
		racc += SPACESHIP_ROTACC;

	if (keystate[(uint8_t)'y'])
		racc -= SPACESHIP_ROTACC;

	if (keystate_special[GLUT_KEY_PAGE_UP])
	{
		// Forward
		m_engine_acc = quatToVector(m_quat)
			* config->getDouble("spaceship_acceleration", 100.0) * USC; // 100km/s^2
		if (!m_engine_running) // powering on
		{
			m_audio.stop();
			m_audio.addFile("spaceship_start.ogg");
			m_audio.setLoop(AL_FALSE);
			m_audio.play();
		}
		m_time_since_acc = 0;
		m_engine_running = true;
	}
	else
	{
		if (m_engine_running) // powering down
		{
			m_audio.stop();
			m_audio.addFile("spaceship_stop.ogg");
			m_audio.setLoop(AL_FALSE);
			m_audio.play();
		}
		m_engine_acc = SimpleVec3d();
		m_engine_running = false;
	}

	if (keystate_special[GLUT_KEY_PAGE_DOWN])
	{
		// Make the spaceship rotate into the opposite direction of the velocity
		glm::quat targetquat = RotationBetweenVectors(SimpleVec3d(0, 0, 1), m_velocity);
		m_quat = glm::normalize(m_quat);
		targetquat = glm::normalize(targetquat);

		if (game->getGameSpeed() > 15.0)
		{
			m_quat = targetquat;
			m_velquat = glm::quat();
		}
		else
		{
			m_quat = glm::slerp(m_quat, targetquat,
				(float)(dtime / SPACESHIP_ALIGN_TIME));
			m_velquat = glm::slerp(glm::quat(), m_velquat,
				(float)(1.0 / SPACESHIP_ROTBREAK_TIME));
		}
	}

	setAcc(yacc, pacc, racc);

	if (keystate[(uint8_t)'c']) // break rotation, stop if gamespeed > 15
	{
		if (game->getGameSpeed() > 15.0)
			m_velquat = glm::quat();
		else
			m_accquat = glm::slerp(glm::quat(), conjugateQuat(m_velquat),
				(float)(1.0 / SPACESHIP_ROTBREAK_TIME));
	}

	if (keystate[(uint8_t)'b'])
	{
		if (m_navigator->getActive())
		{
			// Align to Navigation target
			glm::quat targetquat = m_navigator->getTargetQuat();
			if (game->getGameSpeed() > 15.0)
			{
				m_quat = targetquat;
				m_velquat = glm::quat();
			}
			else
			{
				m_quat = glm::slerp(m_quat, targetquat,
					(float)(dtime / SPACESHIP_ALIGN_TIME));
				m_velquat = glm::slerp(glm::quat(), m_velquat,
					(float)(1.0 / SPACESHIP_ROTBREAK_TIME));
			}
		}
	}
}
