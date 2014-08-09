#include "environment.h"
#include "particle.h"
#include "gamevars.h"
#include "camera.h"
#include "gllibs.h"
#include "shader.h"
#include "player.h"
#include "game.h"
#include "util.h"

/*
	Fire
*/

/**
 * \brief A source that emits FireParticles
 * \param pos The initial position of the FireParticleSource
 * \param dir The direction in which to emit the FireParticle
 * \param spreadangle The angle in radians that the velocity of FireParticles can differ from dir
 * \param minsize The minimum size of a FireParticle
 * \param maxsize The maximum size of a FireParticle
 * \param maxspeed The maximum speed of a FireParticle
 * \param minspeed The minimum speed of a FireParticle
 * \param particle_mintime The minimum time a particle dissolves after
 * \param particle_maxtime The maximum time a particle dissolves after
 * \param shader The shader to use for the particles
 *
 * Particles are randomly created between the above boundaries. The intensity has to be set via
 * setIntensity() after creation.
 */
FireParticleSource::FireParticleSource(SimpleVec3d pos, SimpleVec3d dir, float spreadangle, float minsize, float maxsize, float minspeed, float maxspeed, float particle_mintime, float particle_maxtime, std::string shader) :
m_dir(dir),
m_intensity(0), // has to be set using setIntensity later on
m_num_emitted_particles(0),
m_spreadangle(spreadangle),
m_minsize (minsize),
m_maxsize (maxsize),
m_minspeed(minspeed),
m_maxspeed(maxspeed),
m_particle_mintime(particle_mintime),
m_particle_maxtime(particle_maxtime),
m_shader(shader),
m_num_shouldemit_particles(0)
{
	m_pos = pos;
}

/// Empty
FireParticleSource::~FireParticleSource()
{
	std::cout<<"~FireParticleSource"<<std::endl;
}

void FireParticleSource::render()
{
	// do nothing, not rendered
}

void FireParticleSource::step(float dtime)
{
	m_num_shouldemit_particles += m_intensity * dtime;

	while (m_num_emitted_particles < m_num_shouldemit_particles)
	{
		m_num_emitted_particles++; // number of fire particles emitted

		// determine the properties of the next emitted particle:
		SimpleAngles thisdir(m_dir);
		thisdir.yaw = -thisdir.yaw;
		thisdir.yaw	+= (1.*rand()/RAND_MAX)	* m_spreadangle - m_spreadangle / 2;
		thisdir.pitch	+= (1.*rand()/RAND_MAX)	* m_spreadangle - m_spreadangle / 2;

		float speed = m_minspeed + (1.0 * rand() / RAND_MAX) * (m_maxspeed - m_minspeed);
		float size  = m_minsize  + (1.0 * rand() / RAND_MAX) * (m_maxsize  - m_minsize);
		SimpleVec3d thisvel = SimpleVec3d(thisdir) * speed;

		float exptime = m_particle_mintime +
			(1.0 * rand() / RAND_MAX) * (m_particle_maxtime  - m_particle_mintime);
		exptime *= game->getGameSpeed();
		game->getWorldEnv()->addObject(
			new FireParticle(m_pos, thisvel + m_init_vel, m_shader, size, exptime));
	}
}


/*
	FireParticle:
*/

/**
 * Prepares vertices and indices of the FireParticle
 */
FireParticle::FireParticle(SimpleVec3d pos, SimpleVec3d vel, std::string shader, float size, float exptime) :
m_shader(shader),
m_time(0),
m_size(size),
m_exptime(exptime)
{
	m_pos = pos;
	m_velocity = vel;

	m_vertices[0][0] = -m_size;
	m_vertices[0][1] = -m_size;

	m_vertices[1][0] =  m_size;
	m_vertices[1][1] = -m_size;

	m_vertices[2][0] =  m_size;
	m_vertices[2][1] =  m_size;

	m_vertices[3][0] = -m_size;
	m_vertices[3][1] =  m_size;

	// Indices - fill with numbers 0..3
	for (uint8_t i = 0; i < 4; ++i)
		m_indices[i] = i;
}

/// Empty
FireParticle::~FireParticle()
{
}

void FireParticle::render()
{
	game->getCamera()->getShaderManager()->getShader(m_shader)->addParameterf("size", m_size);
	game->getCamera()->getShaderManager()->getShader(m_shader)->addParameterf("time", m_time);
	game->getCamera()->getShaderManager()->getShader(m_shader)->addParameterf("exptime", m_exptime);
	game->getCamera()->getShaderManager()->requestShader(m_shader);
	glMultMatrixf(glm::value_ptr(m_rotmatrix));

	glEnableClientState(GL_VERTEX_ARRAY);
	{
		glVertexPointer  (2, GL_FLOAT, 0, m_vertices);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, m_indices);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void FireParticle::step(float dtime)
{
	if (m_time > m_exptime or std::isnan (m_time) or m_time < 0)
	{
		m_obsolete = true; // remove this
		return;
	}

	physicalMove(dtime);
	m_time += dtime;

	// Make the particle face the player
	glm::quat rotquat = game->getPlayer()->getLookQuat();
	m_rotmatrix = glm::toMat4(rotquat);
}
