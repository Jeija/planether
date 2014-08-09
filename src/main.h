#ifndef _MAIFN_H
#define _MAIFN_H

class ConfigurationManager;
class StaticEnvironment;
class WorldEnvironment;
class KeyBoard;
class Camera;
class Mouse;
class Game;

/*
	Init Global Variables
*/
Game			*game;
Mouse 			*mouse;
KeyBoard 		*keyboard;
ConfigurationManager	*config;
float			gamespeed;

void initWindow(int argc, char **argv);

void step();
void display();
void destructor();
void onMouseMove(int x, int y);
void onMouseClick(int button, int state, int x, int y);
void onKeyPress(unsigned char key, int x, int y);
void onKeyRelease(unsigned char key, int x, int y);
void onSpecialKeyPress(int key, int x, int y);
void onSpecialKeyRelease(int key, int x, int y);
void onReshape(int width, int height);

#endif
