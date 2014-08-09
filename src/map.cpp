#include "gllibs.hpp"

#include <iostream>
#include <thread>
#include <math.h>
#include <chrono>

#include "planetconfig.hpp"
#include "environment.hpp"
#include "spaceship.hpp"
#include "keyboard.hpp"
#include "drawutil.hpp"
#include "gamevars.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "debug.hpp"
#include "game.hpp"
#include "util.hpp"
#include "map.hpp"

// Returns position of the earth
SimpleVec3d initUniverse(WorldEnvironment *w_env)
{
	SimpleVec3d sunpos = SimpleVec3d(0, 0, 0);

	/********************
		Stars
	********************/
	w_env->addObject(new Star(	SUN_RADIUS, // Our sun
					sunpos,
					SimpleColor(1.0, 1.0, 0.95), SUN_MASS, "sun"));

	w_env->addObject(new Star(	PROXIMA_RADIUS, // Proxima centauri
					sunpos + SimpleVec3d(0, 0, SUN_PROXIMA_DIST),
					SimpleColor(1.0, 1.0, 0.95), PROXIMA_MASS, "proxima"));


	/**********************
		Planets
	**********************/
	w_env->addObject(new Planet(	MERCURY_RADIUS,
					sunpos + SimpleVec3d(MERCURY_DISTANCE, 0, 0),
					sunpos, MERCURY_MASS,
					SimpleVec3d(0, 0, MERCURY_SPEED),
					MERCURY_ROTAXIS, MERCURY_ROTSPEED, "mercury", -1));

	w_env->addObject(new Planet(	VENUS_RADIUS,
					sunpos + SimpleVec3d(VENUS_DISTANCE, 0, 0),
					sunpos, VENUS_MASS,
					SimpleVec3d(0, 0, VENUS_SPEED),
					VENUS_ROTAXIS, VENUS_ROTSPEED, "venus", 50));

	Planet *earth = new Planet(	EARTH_RADIUS,
					sunpos +  SimpleVec3d(EARTH_DISTANCE, 0, 0),
					sunpos, EARTH_MASS,
					SimpleVec3d(0, 0, EARTH_SPEED),
					EARTH_ROTAXIS, EARTH_ROTSPEED, "earth", -1);
	w_env->addObject(earth);

	Planet *moon = new Planet(	MOON_RADIUS,
					earth->getPos() + SimpleVec3d(MOON_EARTH_DISTANCE, 0, 0),
					earth->getPos(), MOON_MASS,
					SimpleVec3d(0, 0, MOON_SPEED),
					MOON_ROTAXIS, MOON_ROTSPEED, "moon", -1);
	moon->addVel(earth->getVel());
	w_env->addObject(moon);

	w_env->addObject(new Planet(	MARS_RADIUS,
					sunpos + SimpleVec3d(MARS_DISTANCE, 0, 0),
					sunpos, MARS_MASS,
					SimpleVec3d(0, 0, MARS_SPEED),
					MARS_ROTAXIS, MARS_ROTSPEED, "mars", -1));

	w_env->addObject(new Planet(	JUPITER_RADIUS,
					sunpos + SimpleVec3d(JUPITER_DISTANCE, 0, 0),
					sunpos, JUPITER_MASS,
					SimpleVec3d(0, 0, JUPITER_SPEED),
					JUPITER_ROTAXIS, JUPITER_ROTSPEED, "jupiter", 50));

	PlanetRing *saturnring =
			new PlanetRing(	SATURN_RADIUS * 2, SATURN_RADIUS / 4, 300.0,
					//radius, width, density
					0.03, 0.2, // Rotation speed minimum - maximum
					200*USC, 600*USC, // Size of asteroids
					0.05); // vertical spreadangle in radians

	w_env->addObject(new Planet(	SATURN_RADIUS,
					sunpos + SimpleVec3d(SATURN_DISTANCE, 0, 0),
					sunpos, SATURN_MASS,
					SimpleVec3d(0, 0, SATURN_SPEED),
					SATURN_ROTAXIS, SATURN_ROTSPEED, "saturn", 50, saturnring));

	PlanetRing *uranusring =
			new PlanetRing(	URANUS_RADIUS * 2, URANUS_RADIUS / 4, 100.0,
					//radius, width, density
					0.01, 0.1, // Rotation speed minimum - maximum
					50*USC, 200*USC, // Size of asteroids
					0.02); // vertical spreadangle in radians

	w_env->addObject(new Planet(	URANUS_RADIUS,
					sunpos + SimpleVec3d(URANUS_DISTANCE, 0, 0),
					sunpos, URANUS_MASS,
					SimpleVec3d(0, 0, URANUS_SPEED),
					URANUS_ROTAXIS, URANUS_ROTSPEED, "uranus", 50, uranusring));

	w_env->addObject(new Planet(	NEPTUNE_RADIUS,
					sunpos + SimpleVec3d(NEPTUNE_DISTANCE, 0, 0),
					sunpos, NEPTUNE_MASS,
					SimpleVec3d(0, 0, NEPTUNE_SPEED),
					NEPTUNE_ROTAXIS, NEPTUNE_ROTSPEED, "neptune", 50));

	w_env->addObject(new TestGrid());

	return earth->getPos();
}


/*
	Star
*/

Star::Star(float radius, SimpleVec3d pos, SimpleColor color, double mass, std::string name) :
PhysicalObject(),
m_radius(radius),
m_color(color),
m_name(name),
m_farmode(true)
{
	m_pos = pos;
	m_mass = mass;
	m_light.enable();
	m_light.setLightID(GL_LIGHT1);
	m_light.addLightInformationcolor(GL_DIFFUSE, m_color);
	m_light.addLightInformationcolor(GL_SPECULAR, m_color);
	m_light.addLightInformationf(GL_CONSTANT_ATTENUATION,	0			);
	m_light.addLightInformationf(GL_LINEAR_ATTENUATION,	0.000000004 / USC	);
	m_light.addLightInformationf(GL_QUADRATIC_ATTENUATION,	0			);
	m_sphere = SphereFraction::makePrototype(m_radius, 5, 5);
	m_sphere->setChildrenStatic(false);

	// Generate corona vertices + indices
	m_corona_vertices[0][0] = -m_radius * 3;
	m_corona_vertices[0][1] = -m_radius * 3;

	m_corona_vertices[1][0] =  m_radius * 3;
	m_corona_vertices[1][1] = -m_radius * 3;

	m_corona_vertices[2][0] =  m_radius * 3;
	m_corona_vertices[2][1] =  m_radius * 3;

	m_corona_vertices[3][0] = -m_radius * 3;
	m_corona_vertices[3][1] =  m_radius * 3;

	// Indices - fill with numbers 0..3
	for (uint8_t i = 0; i < 4; ++i)
		m_corona_indices[i] = i;
}

Star::~Star()
{
	std::cout<<"~"<<m_name<<std::endl;

	delete m_sphere;
}

void Star::render ()
{
	// Sphere
	game->getCamera()->getShaderManager()->requestShader(m_name);
	m_color.set();
	m_color.setEmission();
	m_sphere->render(); // don't use glutSolidSphere as SphereFractions are way faster

	// Corona
	// in front of sun, facing the player
	SimpleVec3d dirvec = (game->getPlayer()->getPos() - m_pos).normalize();

	// move corona away from sun when player is very far away (prevents depthtest errors)
	float distance = getVectorLength(game->getPlayer()->getPos() - m_pos) * m_radius / 40000.0;
	(dirvec * (m_radius + distance)).translate();

	game->getCamera()->getShaderManager()->resetShader();
	game->getCamera()->getShaderManager()->getShader("corona")->addParameterf("radius", m_radius);
	game->getCamera()->getShaderManager()->requestShader("corona");

	glm::quat coronaquat = RotationBetweenVectors(SimpleVec3d(0, 0, -1), dirvec);
	glMultMatrixf(glm::value_ptr(glm::toMat4(coronaquat)));
	glRotatef(180, 0, 1, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	{
		glVertexPointer  (2, GL_FLOAT, 0, m_corona_vertices);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, m_corona_indices);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Star::renderPreview(float time, float scale)
{
	glScalef(scale / m_radius, scale / m_radius, scale / m_radius);

	// Shader Parameter to disable lighting
	game->getCamera()->getShaderManager()->requestShader(m_name);
	glRotatef(time * TELEPORT_PREVIEW_ROTSPEED, 0, 1, 0);

	glutSolidSphere(m_radius, 50, 50);
}

void Star::step (float dtime)
{
	float dist_to_player = getVectorLength(game->getPlayer()->getPos() - m_pos);

	if (dist_to_player > m_radius * 50 && !m_farmode)
	{ // decrease detail level
		m_sphere->setChildren(10, 10);
		m_sphere->updateVertices();
		m_farmode = true;
	}

	if (dist_to_player < m_radius * 49 && m_farmode)
	{ // increase detail level
		m_sphere->setChildren(250, 250);
		m_sphere->updateVertices();
		m_farmode = false;
	}

	physicalMove(dtime);

	// Lose when colliding with a star
	if (getVectorLength(game->getSpaceship()->getPos() - m_pos) < m_radius)
		game->triggerLose();
}

// Planet Teleport Capabilities
std::string Star::getTeleportName  ()
{
	return m_name;
}

SimpleVec3d Star::getTeleportPos   ()
{
	return m_pos - SimpleVec3d(-m_radius * 4, 0, 0);
}


SimpleAngles Star::getTeleportAngles()
{
	return SimpleAngles(SimpleVec3d(1, 0, 0));
}

/*
	Planet
*/

/**
 * \brief Create a new Planet
 * \param radius The radius of the new Planet
 * \param pos The position of the Planet
 * \param gravcen The gravity center the planet should rotate around when pre-rotating the planets.
 * \param mass The mass of the Planet in kg
 * \param vel The initial velocity of the planet
 * \param rotaxis The rotation axis of the planet (around itself)
 * \param rotspeed The speed of the planets rotation in rad/second
 * \param name The name and shader name of the planet
 * \param const_vertexnum Number of children yaw and pitch the planet shall have. -1 if autoChildrenNum.
 * \param ring The ring around the planet
 *
 * gravcen is only used for positioning the planets before the game starts. The Planet will then
 * rotate around the gravity center by a random angle. Therefore pos, velocity only need to be
 * valid for one single position on the orbit. Later on, the planet will rotate based on its
 * attraction by other massive objects (stars, e.g. the sun).
 */
Planet::Planet(	float radius, SimpleVec3d pos, SimpleVec3d gravcen, double mass, SimpleVec3d vel,
		SimpleVec3d rotaxis, float rotspeed, std::string name, int const_vertexnum,
		PlanetRing *ring) :
PhysicalObject(),
m_upd_det_running(false),
m_radius(radius),
m_rotaxis(rotaxis),
m_rotspeed(rotspeed),
m_time(0),
m_name(name),
m_ring(ring)
{
	m_mass = mass;
	m_pos = pos;
	m_velocity = vel;

	// Handle auto-generation of children: Either spawn a updateDetailThread or generate a
	// Sphere with constant number of vertices.
	if (const_vertexnum == -1) // automatic vertex number
	{
		m_pgensphere = SphereFraction::makePrototype(m_radius, 30, 30);
		m_upd_det_running = true;
		m_upd_det_thread = std::thread(&Planet::updateDetailThread, this);
	}
	else
	{
		m_pgensphere = SphereFraction::makePrototype(m_radius,
			const_vertexnum, const_vertexnum);
	}

	if (config->getBool("prerotate_planets", false))
	{
		// Pre-rotate the planet around its mass center
		// The rotation axis of the planet is the crossProduct of its velocity and the vector
		// from the planet to the gravity center by logic.
		SimpleVec3d axis = crossProduct(m_pos - gravcen, m_velocity);
		randomPlanetPosition(&m_pos, &m_velocity, gravcen, axis);
	}
}

Planet::~Planet()
{
	std::cout<<"~"<<m_name<<std::endl;

	if (m_upd_det_running) // automatic vertex number --> stop thread
	{
		m_upd_det_running = false;
		m_upd_det_thread.join();
	}

	delete m_ring;
	delete m_pgensphere;
}

void Planet::render ()
{
	// Do not render planets if they are so far away that they're practically invisible
	if (getVectorLength(game->getPlayer()->getPos() - m_pos) > m_radius * 1000) return;

	// First render the ring while shader is not loaded
	if (m_ring != nullptr) m_ring->render();

	// Use lighting - this is no preview
	game->getCamera()->getShaderManager()->getShader(m_name)->addParameteri("preview", 0);
	game->getCamera()->getShaderManager()->requestShader(m_name);

	glRotatef(RADTODEG(m_time * m_rotspeed), m_rotaxis.x, m_rotaxis.y, m_rotaxis.z);
	m_pgensphere->render();
}

void Planet::renderPreview(float time, float scale)
{
	glScalef(scale / m_radius, scale / m_radius, scale / m_radius);

	// Shader Parameter to disable lighting
	game->getCamera()->getShaderManager()->getShader(m_name)->addParameteri("preview", 2);
	game->getCamera()->getShaderManager()->requestShader(m_name);
	glRotatef(time * TELEPORT_PREVIEW_ROTSPEED, m_rotaxis.x, m_rotaxis.y, m_rotaxis.z);

	glutSolidSphere(m_radius, 50, 50);
}

void Planet::step (float dtime)
{
	m_acceleration = game->getWorldEnv()->getGravityAcc(m_pos);
	physicalMove(dtime);
	m_time += dtime;

	// only if the player is nearby, step the ring
	if (getVectorLength(game->getPlayer()->getPos() - m_pos) < m_radius * 1000)
	{
		if (m_ring != nullptr) m_ring->step(dtime);
	}

	// Losing when colliding with planets
	if (getVectorLength(game->getSpaceship()->getPos() - m_pos) < m_radius)
		game->triggerLose();
		//game->getSpaceship()->setVelocity(game->getSpaceship()->getVelocity() * -1);
}

void Planet::updateDetailThread()
{
	while(m_upd_det_running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(400));

		if (game == NULL) continue; // not yet initialized

		SimpleVec3d position_relative = game->getPlayer()->getPos() - m_pos;
		position_relative = position_relative.rotateBy(m_rotaxis, -m_rotspeed * m_time);

		if (m_pgensphere->autoChildrenNum(position_relative, 60.0))
			m_pgensphere->updateVertices();
	}
}

// Planet Teleport Capabilities
std::string Planet::getTeleportName  ()
{
	return m_name;
}

SimpleVec3d Planet::getTeleportPos   ()
{
	return m_pos - SimpleVec3d(m_radius * 4, 0, 0);
}


SimpleAngles Planet::getTeleportAngles()
{
	return SimpleAngles(SimpleVec3d(-1, 0, 0));
}

/*
	PlanetRing
*/
PlanetRing::PlanetRing(float radius, float width, float density, float rotspeed_min,
	float rotspeed_max, float minsize, float maxsize, float vertical_spread) :
m_radius(radius),
m_width(width),
m_density(density)
{
	for (float inclination = 0; inclination < PI*2; inclination += 1.0 / m_density)
	{
		struct RingAsteroid asteroid;
		asteroid.position = SphericalVector3f(
			m_radius + m_width * 2 * (1. * rand() / RAND_MAX - 0.5),
			inclination, (1. * rand() / RAND_MAX - 0.5) * vertical_spread);
		asteroid.size = minsize  + (maxsize - minsize) * (1.  * rand() / RAND_MAX);
		asteroid.rotspeed = rotspeed_min
			+ (rotspeed_max - rotspeed_min) * (1.  * rand() / RAND_MAX);
		m_asteroids.push_back(asteroid);
	}

	m_dodecahedron_displist = glGenLists(1);

	glNewList(m_dodecahedron_displist, GL_COMPILE);
	{
		glutSolidDodecahedron();
	}
	glEndList();
}

PlanetRing::~PlanetRing()
{
	std::cout<<"~Planet"<<std::endl;
	glDeleteLists(m_dodecahedron_displist, 1);
}

void PlanetRing::step(float dtime)
{
	for (std::vector<RingAsteroid>::iterator it = m_asteroids.begin();
		it != m_asteroids.end(); ++it)
		it->position.inclination += dtime * it->rotspeed;
}

void PlanetRing::render()
{
	for (auto asteroid : m_asteroids)
	{
		glPushMatrix();
		{
			game->getCamera()->getShaderManager()->requestShader("asteroid");
			game->getCamera()->getShaderManager()->getShader("asteroid")->addParameterf
				("scale", asteroid.size);

			SimpleVec3d trans = SimpleVec3d(asteroid.position);
			glTranslatef(trans.x, trans.y, trans.z);

			glCallList(m_dodecahedron_displist);

			game->getCamera()->getShaderManager()->resetShader();
		}
		glPopMatrix();
	}
}

/*
	TestGrid / Coordinate Grid
*/

TestGrid::TestGrid() :
m_draw(false)
{
	keyboard->registerKeyPressCallback(TestGrid::onKeyboard, this);
}

TestGrid::~TestGrid()
{
	std::cout<<"~TestGrid"<<std::endl;
}

void TestGrid::onKeyboard(unsigned char key, void *param)
{
	if (game->getTportOverlay()) return;

	if (key == 'g')
		((TestGrid*)param)->toggleDraw();
}

float TestGrid::getNextGridPos(float num, float gridsize)
{
	int offset = floor(num/gridsize); // number of grid elements
	return num - offset*gridsize; // length of offset from next grid point
}

void TestGrid::step(float dtime)
{
	if (!m_draw) return;
	m_pos = SimpleVec3d(
		game->getPlayer()->getPos().x -
			getNextGridPos(game->getPlayer()->getPos().x, GRIDLEN),
		game->getPlayer()->getPos().y -
			getNextGridPos(game->getPlayer()->getPos().y, GRIDLEN),
		game->getPlayer()->getPos().z -
			getNextGridPos(game->getPlayer()->getPos().z, GRIDLEN)
	);
}

#define GR_AXTHICK LMIN	* 0.0004 // grid axis thickness
#define GR_AXLEN LMIN	*  0.006  // grid axis length
#define GRIDMIN (-GRIDLEN*GRIDSIZE/2.0+GRIDLEN)
void TestGrid::render ()
{
	if (!m_draw) return;
	SimpleVec3d(GRIDMIN, GRIDMIN, GRIDMIN).translate();

	SimpleColor(1, 1, 1, 1).setEmission();
	double x, y, z;
	for (x = 0; x<GRIDLEN*GRIDSIZE; x+=GRIDLEN)
	{
		for (y = 0; y<GRIDLEN*GRIDSIZE; y+=GRIDLEN)
		{
			for (z = 0; z<GRIDLEN*GRIDSIZE; z+=GRIDLEN)
			{
				glPushMatrix();
				{
					SimpleVec3d(x, y, z).translate();
					// x-Axis
					SimpleVec3d p3(-GR_AXLEN, -GR_AXTHICK, -GR_AXTHICK);
					SimpleVec3d p4( GR_AXLEN,  GR_AXTHICK,  GR_AXTHICK);
					makeCuboid(p3, p4, 1, 1, 1);

					// y-Axis
					SimpleVec3d p5(-GR_AXTHICK, -GR_AXLEN, -GR_AXTHICK);
					SimpleVec3d p6( GR_AXTHICK,  GR_AXLEN,  GR_AXTHICK);
					makeCuboid(p5, p6, 1, 1, 1);

					// z-Axis
					SimpleVec3d p1(-GR_AXTHICK, -GR_AXTHICK, -GR_AXLEN);
					SimpleVec3d p2( GR_AXTHICK,  GR_AXTHICK,  GR_AXLEN);
					makeCuboid(p1, p2, 1, 1, 1);
				}
				glPopMatrix();
			}
		}
	}
}



