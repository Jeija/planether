#include <iostream>
#include "gllibs.hpp"

#include "environment.hpp"
#include "gamevars.hpp"
#include "player.hpp"
#include "config.hpp"
#include "tests.hpp"
#include "game.hpp"
#include "util.hpp"

void mouse_trigger_shot(int button, int state, int x, int y, void *unused)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		game->getWorldEnv()->addObject(new Bullet(game->getPlayer()));
}

Bullet::Bullet (Player *player) :
PhysicalObject(),
m_time(0)
{
	m_pos = player->getPos() + player->getLookAxis() * 10;
	m_velocity = player->getLookAxis() / 10;
	m_acceleration = SimpleVec3d(0, -9.81 * USC * 1000, 0);

	m_light.setLightID(GL_LIGHT0);
	m_light.addLightInformationcolor(GL_DIFFUSE, SimpleColor(0.9, 0.9, 0.8, 1.0));
	m_light.addLightInformationf (GL_QUADRATIC_ATTENUATION, 0.001);
}

Bullet::~Bullet()
{
	std::cout<<"~Bullet"<<std::endl;
}

void Bullet::render ()
{
	SimpleAngles angles = SimpleAngles(SimpleVec3d(m_velocity.x, m_velocity.y, m_velocity.z));
	glRotatef(angles.yaw   / PI * 180, 0.0f, 0.1f, 0.0f);
	glRotatef(angles.pitch / PI * 180, 0.1f, 0.0f, 0.0f);

	glColor3f(1, 1, 1);
	SimpleColor(1, 1, 1).setEmission();
	glutSolidCube(500 * USC);
}

void Bullet::step (float dtime)
{
	physicalMove(dtime);
	m_time += dtime;
	if (m_time > 20) m_obsolete = true;
}
