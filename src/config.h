/*
	This file contains both hardcoded configuration options as well as dynamically
	loaded configuration.
*/
#include "rapidjson/document.h"

#ifndef CONFIG_H
#define CONFIG_H

/// Loads the JSON configuration file and provides access to the values
class ConfigurationManager
{
	public:
		ConfigurationManager();
		~ConfigurationManager();

		std::string getString	(std::string property, std::string def);
		int getInt		(std::string property, int def);
		bool getBool		(std::string property, bool defe);
		double getDouble	(std::string property, double def);

	private:
		rapidjson::Document m_json;
};

/*
	Platform
*/

#if defined(WIN32) || defined(_WIN32)
	#define PLANETHER_WINDOWS
	#define DIR_DELIM "\\"
#else 
	#define DIR_DELIM "/"
#endif

/*
	Universe
*/
#define UNISCALE 0.01 // 1km in RL --> 0.01 in simulation
#define USC UNISCALE
#define LIGHTMINUTE (17987547.48*USC)
#define LMIN LIGHTMINUTE
#define LIGHTYEAR (9460700000000.0*USC)

// Enable = 1 / Disable = 0
#define ENABLE_GRAVITY  1
#define ENABLE_MOVEMENT 1
#define ENABLE_ROTATION 1

// Gravitational constant
#define GRAV_CONST 0.00000000006673

// Map: TestGrid size
#define GRIDLEN (LMIN) // Distance between grid elements
#define GRIDSIZE 9 // Number of grid elements in one dimension 
/*
	General
*/
#define APPLICATION_NAME "Planether"
#define INITIAL_GAMESPEED 1
#define GAMESPEED_MAX 10000000
#define GAMESPEED_MIN 0.01
#define GAMESPEED_CHANGE_QUAD 1
#define GAMESPEED_CHANGE_LIN  4
#define SPEED_OF_LIGHT 299792.458
#define TELEPORT_PREVIEW_ROTSPEED 60.0 // degrees per second

/*
	Screen Resolution  / Graphics
*/
#define SCREEN_W 1024
#define SCREEN_H 800
#define ENABLE_CROSSHAIR TRUE
#define FPS_UPDATETIME 100
#define BACKPLANE 100000000000*USC

/*
	Mouse (MOUSE_SENSITIVITY sensitivity in radians per pixel)
*/
#define MOUSE_SENSITIVITY 0.001
#define MOUSE_ROLL_SENSITIVITY 0.04
#define MOUSE_BUTTON_ROLLSPEED 0.1

/*
	Shaders
*/
#define SHADER_DIR "shaders" DIR_DELIM
#define SHADER_BUILTIN_FILENAME "builtin.glsl"
#define BUILTIN_SHADER_PATH SHADER_DIR SHADER_BUILTIN_FILENAME

/*
	Config file
*/
#define CONFIG_DIR "config" DIR_DELIM
#define CONFIG_FILENAME "config.json"
#define CONFIG_PATH CONFIG_DIR CONFIG_FILENAME

/*
	Audio
*/
#define SOUNDS_DIR "sounds" DIR_DELIM
#define SPEED_OF_SOUND 0.340 * USC
#undef  USE_ALUT // do not use ALut, no .wav loading

/*
	Graphics
*/
#define USE_SKYBOX_SHADER 0

/*
	SpaceShip
*/
#define SPACESHIP_ROTACC 1.0 / 100.0
#define SPACESHIP_ROTBREAK_TIME 2.0
#define SPACESHIP_ALIGN_TIME 0.5
#define SPACESHIP_FOLLOWSPEED 10.0
#define GEOSTATIONARY_SPEED 3.0475 * USC
#define GEOSTATIONARY_DISTANCE 42164 * USC
#define GEOSTATIONARY_ROTSPEED (-2*PI/(24*3600)/10)
// GEOSTATIONARY_ROTSPEED: / 10, because velquats work with dtime * 10


/*
|H=y /
|   / L=z
|  /
| /
|/___________
      W=x
*/


#endif
