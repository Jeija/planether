#include "gllibs.hpp"
#include "objects.hpp"
#include "util.hpp"

#ifndef _PARTICLE_H
#define _PARTICLE_H

/// Emits FireParticle in a random fashion
class FireParticleSource : public WorldObject
{
	public:
		FireParticleSource(SimpleVec3d pos, SimpleVec3d dir,
			float spreadangle, float minsize, float maxsize, float minspeed,
			float maxspeed, float particle_mintime, float particle_maxtime,
			std::string shader);
		~FireParticleSource();


		void render();
		void step(float dtime);

		/// Remove the FireParticleSource
		void remove()
			{ m_obsolete = true; }

		/// Update the position of the FireParticleSource
		void setPos(SimpleVec3d pos)
			{ m_pos = pos; }

		/// Update the direction of particles the be emitted
		void setDir(SimpleVec3d dir)
			{ m_dir = dir; }

		/// Set Intensity of particle creation in particles / second
		void setIntensity(int intensity)
			{ m_intensity = intensity; }

		/// Set initial velocity of the particles
		void setInitialVelocity(SimpleVec3d vel)
			{ m_init_vel = vel; }

	private:
		SimpleVec3d m_dir;
		int m_intensity;
		int m_num_emitted_particles;
		float m_spreadangle;
		float m_minsize;
		float m_maxsize;
		float m_minspeed;
		float m_maxspeed;
		float m_particle_mintime;
		float m_particle_maxtime;
		std::string m_shader;
		float m_num_shouldemit_particles;
		// velocity that particles already have when being sent out (moving source)
		SimpleVec3d m_init_vel;
};

/// Single FireParticle, a particle object that can be colored with a shader
class FireParticle : public PhysicalObject
{
	public:
		FireParticle(SimpleVec3d pos, SimpleVec3d vel, std::string shader, float size,
			float exptime);
		~FireParticle();

		void render();
		void step(float dtime);

	private:
		GLfloat m_vertices[4][2];
		GLubyte m_indices [4];

		std::string m_shader;
		float m_time;
		float m_size;
		float m_exptime;
		glm::mat4 m_rotmatrix;
};

#endif
