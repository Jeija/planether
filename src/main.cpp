#include <assert.h>
#include <iostream>
#include "gllibs.hpp"
#include <string>
#include <vector>
#include <time.h>

#include "environment.hpp"
#include "gamevars.hpp"
#include "keyboard.hpp"
#include "splash.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "mouse.hpp"
#include "main.hpp"
#include "game.hpp"

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

/**
 * \brief Restarts the game when lost
 *
 * Checks if triggerLose has been called on the game. If so, deletes the game and restarts it.
 * Therefore, this can only be a static member of Game.
 */
void checklose()
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

/*
	Wrappers to Game class member functions
*/
void step()
{
	checklose();
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
