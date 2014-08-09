#include <assert.h>
#include <iostream>
#include "gllibs.h"
#include <string>
#include <vector>
#include <time.h>

#include "planetconfig.h"
#include "environment.h"
#include "spaceship.h"
#include "quatutil.h"
#include "gamevars.h"
#include "keyboard.h"
#include "shader.h"
#include "skybox.h"
#include "camera.h"
#include "config.h"
#include "player.h"
#include "splash.h"
#include "tests.h"
#include "audio.h"
#include "mouse.h"
#include "main.h"
#include "util.h"
#include "game.h"
#include "hud.h"
#include "map.h"

/**
 * \brief Main function - starts the game
 *
 * Starts up ConfigurationManager, KeyBoard, Mouse and the game itself. Calls
 * initWindow to initialize the GLUT window.
 */
int main(int argc, char **argv)
{
	std::cout<<"Starting the Game!"<<std::endl;
#ifdef PLANETHER_WINDOWS
	std::cout<<"###########################"<<std::endl;
	std::cout<<"##        WARNING        ##"<<std::endl;
	std::cout<<"##-----------------------##"<<std::endl;
	std::cout<<"##  You are running the  ##"<<std::endl;
	std::cout<<"## MS Windows Version of ##"<<std::endl;
	std::cout<<"##  Planether that only  ##"<<std::endl;
	std::cout<<"## has limited function- ##"<<std::endl;
	std::cout<<"##         ality         ##"<<std::endl;
	std::cout<<"###########################"<<std::endl;
	std::cout<<std::endl;
#endif
	srand(time(NULL));
	config = new ConfigurationManager();

	initWindow(argc, argv);

	keyboard = new KeyBoard();
	mouse = new Mouse();
	game = new Game();
	game->init();
	atexit(destructor);

	glutMainLoop();

	return 1;
}

/**
 * \brief Initializes the GLUT window
 *
 * Initializes GLUT and GLEW, creates a window and draws the splashscreen.
 * Also registers the GLUT callbacks for keyboard, mouse, idle, drawing etc.
 */
void initWindow(int argc, char **argv)
{
	// General Init / Graphics (GLUT stuff)
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(APPLICATION_NAME);

	// Draw Splashcreen
	drawSplashScreen();

	// Glew:
	GLenum glewstat = glewInit();
	if (glewstat != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(glewstat) << std::endl;
		return;
	}

	// Callbacks
	glutDisplayFunc		(display);
	glutIdleFunc		(step);
	glutKeyboardFunc	(onKeyPress);
	glutKeyboardUpFunc	(onKeyRelease);
	glutSpecialFunc		(onSpecialKeyPress);
	glutSpecialUpFunc	(onSpecialKeyRelease);
#ifndef PLANETHER_WINDOWS // (otherwise handled by windows.h)
	glutMotionFunc		(onMouseMove);
	glutPassiveMotionFunc	(onMouseMove);
#endif
	glutMouseFunc		(onMouseClick);
	glutReshapeFunc		(onReshape);
}

/**
 * \brief Deletes the global instances
 *
 * Deletes game, mouse, keyboard and config in the given order.
 */
void destructor()
{
	delete game;
	delete mouse;
	delete keyboard;
	delete config;
}

/*
	Wrappers to Game class member functions
*/
void step()
{
	Game::checklose();
	game->step();
}

void display()
{
	game->getCamera()->render();
}

void onMouseMove(int x, int y)
{
	mouse->onMouseMove(x, y);
}

void onMouseClick(int button, int state, int x, int y)
{
	mouse->onMouseClick(button, state, x, y);
}

void onKeyPress(unsigned char key, int x, int y)
{
	keyboard->onKeyPress(key, x, y);
}

void onKeyRelease(unsigned char key, int x, int y)
{
	keyboard->onKeyRelease(key, x, y);
}

void onSpecialKeyPress(int key, int x, int y)
{
	keyboard->onSpecialKeyPress(key, x, y);
}

void onSpecialKeyRelease(int key, int x, int y)
{
	keyboard->onSpecialKeyRelease(key, x, y);
}

void onReshape(int width, int height)
{
	game->getStaticEnv()->reshape(width, height);
}

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
 * \brief Restarts the game when lost
 *
 * Checks if triggerLose has been called on the game. If so, deletes the game and restarts it.
 * Therefore, this can only be a static member of Game.
 */
void Game::checklose()
{
	if (game->getLost())
	{
		drawLoseScreen();
		delete keyboard;
		delete mouse;
		delete game;
		keyboard = new KeyBoard();
		mouse = new Mouse();
		game = new Game();
		game->init();
	}
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
