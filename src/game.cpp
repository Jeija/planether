#include "planetconfig.hpp"
#include "environment.hpp"
#include "spaceship.hpp"
#include "quatutil.hpp"
#include "gamevars.hpp"
#include "keyboard.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "player.hpp"
#include "tests.hpp"
#include "audio.hpp"
#include "mouse.hpp"
#include "util.hpp"
#include "game.hpp"
#include "hud.hpp"
#include "map.hpp"

/***********************
	Game
***********************/

/**
 * \brief Creates a new instance of Game
 *
 * Prepares WorldEnvironment, StaticEnvironment and AudioEnvironment
 * to be populated. The game will be only be actually launched when
 * Game::init() is called. That populates the Environments with the
 * all the objects and connects callbacks etc.
 */
Game::Game() :
m_crosshair(new CrossHair()),
m_hud_physics(new PhysicsInformation),
m_hud_planetloc(new PlanetLocator(this)),
m_tport_overlay(false),
m_seed(config->getInt("seed", 4)),
m_wireframe(false),
m_lost(false),
m_speed(INITIAL_GAMESPEED),
m_time(0),
m_time_real(0)
{
	// Environment
	m_world_env = new WorldEnvironment();
	m_static_env = new StaticEnvironment();
	m_audio_env = new AudioEnvironment();
}

/**
 * \brief Destructs Game
 *
 * Deletes the private instances of the Game such as the environments and the camera, however
 * not those, that are contained within any of the environments (e.g. SpaceShip)
 */
Game::~Game()
{
	//delete m_spaceship; (WorldObject will be removed by WorldEnvironment)
	delete m_audio_env;
	delete m_world_env;
	delete m_static_env;
	delete m_background_music;
	delete m_cam->getSkyBox();
	delete m_cam->getShaderManager();
	delete m_cam;
}

/**
 * \brief Initializes the game
 *
 * Game::Game() must have prepared the game before.
 * Connects mouse + keyboard callbacks and creates Player, Camera, SpaceShip,
 * BackgroundMusicManager. After this, the Game should be ready to play.
 */
void Game::init()
{
	// Camera position is determined by Player
	m_player = new Player(SimpleVec3d(EARTH_DISTANCE * 0.9999, 0, 0), SimpleAngles(0, 0, 0));
	m_cam = new Camera(m_world_env, m_static_env, m_player, new ShaderManager(), new SkyBox());	

	// Controls / Inputs
	mouse->registerClickCallback(on_mouse_click_default, nullptr);
	keyboard->registerCallback(process_keyboard_layout_default);
	keyboard->registerKeyPressCallback(process_keypress_default);

	// Background Music
	m_background_music = new BackgroundMusicManager();

	// Actually initialize the game
	m_static_env->addObject(m_crosshair);
	m_static_env->addObject(m_hud_physics);
	m_static_env->addObject(m_hud_planetloc);
	SimpleVec3d earthpos = initUniverse(m_world_env); // World Map

	/*
		Spaceship
	*/
	// Geostationary rotation:
	glm::quat dirquat = RotationBetweenVectors(SimpleVec3d(0.0, 0.0, -1.0), earthpos);
	glm::quat velquat = anglesToQuat(SimpleAngles(0.0, GEOSTATIONARY_ROTSPEED, 0.0));

	SimpleVec3d spaceshipspeed = crossProduct(earthpos, SimpleVec3d(0, 1, 0)).normalize()
		* EARTH_SPEED;
	spaceshipspeed += crossProduct(earthpos, spaceshipspeed * -1).normalize() * GEOSTATIONARY_SPEED;
	m_spaceship = new SpaceShip(earthpos.normalize() * (EARTH_DISTANCE - GEOSTATIONARY_DISTANCE),
		spaceshipspeed, dirquat, velquat);

	m_world_env->addObject(m_player);
	m_world_env->addObject(m_spaceship);

	// Make cursor invisible when everything is done
	glutSetCursor(GLUT_CURSOR_NONE);

	// Just for testing
	// mouse->registerClickCallback(mouse_trigger_shot, nullptr);
}

/**
 * \brief Performs a step on the game
 *
 * To be called by Game::step_wrapper(), which is registered by Game::init(). Executed when GLUT
 * is in idle (glutIdleFunc()). Calculates dtime (delta time), handles keyboard + mouse (MS Windows)
 * input and calls step on the environments. Posts a redisplay via glutPostRedisplay().
 */
void Game::step()
{
	/*
		Timing
	*/
	float dtime = (glutGet(GLUT_ELAPSED_TIME) / 1000. - m_time_real) * m_speed;
	m_time += dtime;
	m_time_real = glutGet(GLUT_ELAPSED_TIME) / 1000.;

	/*
		Input
	*/
	keyboard->processKeys(dtime);
#ifdef PLANETHER_WINDOWS // process mouse using windows.h
	mouse->handleMicrosoftWindows();
#endif

	/*
		Process data - step environments
	*/
	if (!game->getTportOverlay()) m_world_env->step(dtime); // Pause world when GUI is open
	m_static_env->step(dtime);
	m_audio_env->step();
	m_background_music->step();

	// Put mouse pointer back - just in case it got out of the window_h
	// (glutPassiveMotionFunc won't spot it in that case)
	if (game->getCamera()->getCaptureMouse())
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);

	/*
		Output - post redisplay for the whole scene
	*/
	glutPostRedisplay();
}

/**
 * \brief Adds a value to the game speed.
 * \param val The value to add
 *
 * If value exceeds the bounds of GAMESPEED_MAX or GAMESPEED_MIN, the game speed is set
 * to these extremums.
 */
void Game::addGameSpeed(float val)
{
	m_speed += val;
	if (m_speed > GAMESPEED_MAX) m_speed = GAMESPEED_MAX;
	if (m_speed < GAMESPEED_MIN) m_speed = GAMESPEED_MIN;
}
