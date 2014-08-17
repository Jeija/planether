#ifndef GAMEVARS_H
#define GAMEVARS_H

class ConfigurationManager;
class KeyBoard;
class Mouse;
class Game;

// Cannot be members of the Game class due to limitations of GLUT
// (rendering/mouse/keyboard callbacks must be static, but GLUT is no C++ lib)
// Therefore made public to allow easy access
extern Game			*game;
extern Mouse			*mouse;
extern KeyBoard			*keyboard;
extern ConfigurationManager	*config;

#endif
