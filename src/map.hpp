#ifndef MAP_H
#define MAP_H

#include "util.hpp"
#include "objects.hpp"
#include "teleport.hpp"

class WorldEnvironment;
class Player;
class SphereFraction;

// Returns position of the earth
SimpleVec3d initUniverse(WorldEnvironment *w_env);

/// A glowing celestial body
class Star : public PhysicalObject, public TeleportTarget, public MassObject
{
	public:
		Star(float radius, SimpleVec3d pos, SimpleColor color, double mass,
				std::string name);
		~Star();

		void render();
		void renderPreview(float time, float scale);
		void step(float dtime);

		std::string	getTeleportName  ();
		SimpleVec3d	getTeleportPos   ();
		SimpleAngles	getTeleportAngles();

	private:
		float m_radius;
		SphereFraction *m_sphere;
		SimpleColor m_color;
		std::string m_name;
		bool m_farmode; // drawing mode for being far away

		// Corona:
		GLfloat m_corona_vertices[4][2];
		GLubyte m_corona_indices[4];
};

/// A single asteroid in a PlanetRing
struct RingAsteroid
{
	SphericalVector3f position;
	float size;
	float rotspeed;
};

/// The ring around a Planet
class PlanetRing
{
	public:
		PlanetRing(float radius, float width, float density, float rotspeed_min,
			float rotspeed_max, float minsize, float maxsize, float vertical_spread);
		~PlanetRing();

		void step(float dtime);
		void render();

	private:
		float m_radius;
		float m_width;
		float m_density;

		std::vector<RingAsteroid> m_asteroids;
		GLuint m_dodecahedron_displist;
};

/// A non-glowing, moving celestial body that has support for procedural generation
class Planet : public PhysicalObject, public TeleportTarget, public MassObject
{
	public:
		Planet(float radius, SimpleVec3d pos, SimpleVec3d gravcen, double mass,
			SimpleVec3d vel, SimpleVec3d rotaxis, float rotspeed, std::string name,
			int constant_vertexnum, PlanetRing *ring = nullptr);
		~Planet();
		// if constant_vertexnum == -1 --> autoUpdateThread,
		// else use number as children number (num * num)

		void render();
		void renderPreview(float time, float scale);
		void step(float dtime);

		std::string	getTeleportName  ();
		SimpleVec3d	getTeleportPos   ();
		SimpleAngles	getTeleportAngles();

		SimpleVec3d getPos()
			{ return m_pos; }
		SimpleVec3d getVel()
			{ return m_velocity; }
		void addVel(SimpleVec3d vel)

			{ m_velocity += vel; }
	private:
		/*
			updateDetailThread updates the detail level of the Planet,
			adds and removes vertices based on the camera position
		*/
		void updateDetailThread();
		bool m_upd_det_running;
		std::thread m_upd_det_thread;

		float m_radius;
		SphereFraction *m_pgensphere;

		SimpleVec3d m_rotaxis;
		float m_rotspeed;
		float m_time;

		std::string m_name;

		PlanetRing *m_ring;
};

/// A coordinate grid that can be displayed by pressing 'g', provides some reference for large distances
class TestGrid : public WorldObject
{
	public:
		TestGrid();
		~TestGrid();

		void render();
		void step(float dtime);
		static void onKeyboard(unsigned char key, void *param);
		void toggleDraw()
			{ m_draw = !m_draw; };

	private:
		float getNextGridPos(float num, float gridsize);
		bool m_draw;
};

#endif
