#include <iostream>
#include <math.h>
#include "gllibs.hpp"

#include "environment.hpp"
#include "keyboard.hpp"
#include "gamevars.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "config.hpp"
#include "mouse.hpp"
#include "util.hpp"
#include "game.hpp"
#include "hud.hpp"

void process_keyboard_layout_default(bool keystate[255], bool keystate_special[255], float dtime, void *param /* unused */)
{
	if (game->getTportOverlay()) return; // keypresses used by TeleportWindow

	if (keystate[(int)*"f"]) // f = FullScreen
	{
		glutFullScreen();
		keystate[(int)*"f"] = false;
	}

	if (keystate[(int)*"F"]) // F = exit FullScreen
	{
		glutPositionWindow(0, 0);
		glutReshapeWindow(SCREEN_W, SCREEN_H);
		keystate[(int)*"F"] = false;
	}

	if (keystate[0x1b]) // Escape = 0x1b
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		game->getCamera()->setCaptureMouse(false);
	}

	if (keystate[(int)*"+"])
	{
		game->addGameSpeed(dtime * (GAMESPEED_CHANGE_QUAD + GAMESPEED_CHANGE_LIN / game->getGameSpeed()));
	}

	if (keystate[(int)*"-"])
	{
		game->addGameSpeed(-dtime * (GAMESPEED_CHANGE_QUAD + GAMESPEED_CHANGE_LIN / game->getGameSpeed()));
	}

	if (keystate[(int)*"t"])
	{
		game->getStaticEnv()->addObject(new TeleportWindow());
	}

	if (keystate[(int)*"q"])
		glutLeaveMainLoop();
}

void process_keypress_default(unsigned char key, void *param)
{
	if (game->getTportOverlay()) return; // keypresses used by TeleportWindow

	if (key == (unsigned char)'p')
		game->setSeed(game->getSeed() + 1);

	if (key == (unsigned char)'o')
		game->setSeed(game->getSeed() - 1);

	if (key == (unsigned char)'i')
		game->setWireframe(!game->getWireframe());
}

/*
	Keyboard Class
*/

/**
 * \brief Create a new KeyBoard initializing all the required values
 */
KeyBoard::KeyBoard() :
m_keystate {false},
m_keystate_special {false},
m_callbacks_ind(0)
{
}

/**
 * \brief Destructor for KeyBoard. Empty.
 */
KeyBoard::~KeyBoard()
{
	std::cout<<"~KeyBoard"<<std::endl;
}

/*
	Register callbacks
*/

/**
 * \brief Register a function to be called every step with information of the KeyBoard
 *
 * \param callback The function to be called
 * \param param A pointer to anything that will be passed on the the function
 *
 * The function gets a list of pressed keys (keystate[255]) and a list of pressed special
 * keys (keystate_special[255]) as well as the delta time value (dtime) and the pointer
 * (param) provided to the registration function. The pointer can be used to store a
 * reference to a class so that a member function can be called via a wrapper.
 *
 * Whether a key is pressed or not can be retrieved by reading the value of
 * keystate[key] (true = pressed, false = released), e.g. keystate[(uint8_t)'a'] or in case of
 * a special key by reading keystate_special[GLUT_KEY_*] in the same fashion.
 */
uint32_t KeyBoard::registerCallback(void (*callback) (bool keystate[255], bool keystate_special[255],
			float dtime, void *param), void *param)
{
	m_callbacks[++m_callbacks_ind] =
		std::pair<void (*)(bool keystate[255], bool keystate_special[255],
		float dtime, void *), void *>(callback, param);
	return m_callbacks_ind;
}

/**
 * \brief Register a callback for a key press event
 * \param callback The function to be called
 * \param param See KeyBoard::registerCallback()
 * \return A handle that can be used to KeyBoard::unRegister() the callback
 *
 * The function gets the unsigned char key that has been pressed.
 */
uint32_t KeyBoard::registerKeyPressCallback(void (*callback) (unsigned char key, void *param), void *param)
{
	m_key_press_callbacks[++m_callbacks_ind] =
		std::pair<void (*)(unsigned char key, void *), void *>
		(callback, param);
	return m_callbacks_ind;
}

/**
 * \brief Register a callback for a key release event
 * \param callback The function to be called
 * \param param See KeyBoard::registerCallback()
 * \return A handle that can be used to KeyBoard::unRegister() the callback
 *
 * The function gets the unsigned char key that has been released.
 */
uint32_t KeyBoard::registerKeyReleaseCallback(void (*callback) (unsigned char key, void *param), void *param)
{
	m_key_release_callbacks[++m_callbacks_ind] =
		std::pair<void (*)(unsigned char key, void *), void *>
		(callback, param);;
	return m_callbacks_ind;
}

/**
 * \brief Register a callback for a special key press event
 * \param callback The function to be called
 * \param param See KeyBoard::registerCallback()
 * \return A handle that can be used to KeyBoard::unRegister() the callback
 *
 * The function gets the int key that has been pressed.
 */
uint32_t KeyBoard::registerSpecialKeyPressCallback(void (*callback) (int key, void *param), void *param)
{
	m_special_key_press_callbacks[++m_callbacks_ind] =
		std::pair<void (*)(int key, void *), void *>
		(callback, param);;
	return m_callbacks_ind;
};

/**
 * \brief Register a callback for a special key press event
 * \param callback The function to be called
 * \param param See KeyBoard::registerCallback()
 * \return A handle that can be used to KeyBoard::unRegister() the callback
 *
 * The function gets the int key that has been released.
 */
uint32_t KeyBoard::registerSpecialKeyReleaseCallback(void (*callback)(int key, void *param), void *param)
{
	m_special_key_release_callbacks[++m_callbacks_ind] =
		std::pair<void (*)(int key, void *), void *>
		(callback, param);; 
	return m_callbacks_ind;
};

/**
 * \brief Removes a callback
 * \param id The handle retrieved by KeyBoard::register*()
 *
 * Removes the callback from the list so that it won't be called anymore.
 */
void KeyBoard::unRegister(uint32_t id)
{
	// If key exists, remove it from all the callbacks
	if (m_callbacks.find(id) != m_callbacks.end())
		m_callbacks.erase(id);
	if (m_key_press_callbacks.find(id) != m_key_press_callbacks.end())
		m_key_press_callbacks.erase(id);
	if (m_key_release_callbacks.find(id) != m_key_release_callbacks.end())
		m_key_release_callbacks.erase(id);
	if (m_special_key_press_callbacks.find(id) != m_special_key_press_callbacks.end())
		m_special_key_press_callbacks.erase(id);
	if (m_special_key_release_callbacks.find(id) != m_special_key_release_callbacks.end())
		m_special_key_release_callbacks.erase(id);
}

/*
	Execute callbacks on action
*/
/// Call callbacks registered using KeyBoard::registerCallback(), stepper function 
void KeyBoard::processKeys(float dtime)
{
	for (auto callback : m_callbacks)
	{
		  callback.second.first(m_keystate, m_keystate_special, dtime,
			callback.second.second);
	}
}

/// Call callbacks registered using KeyBoard::registerKeyPressCallback() 
void KeyBoard::onKeyPress(unsigned char key, int x, int y)
{
	m_keystate[key] = true;

	for (auto callback : m_key_press_callbacks)
	{
		  callback.second.first(key, callback.second.second);
	}
}

/// Call callbacks registered using KeyBoard::registerKeyReleaseCallback() 
void KeyBoard::onKeyRelease(unsigned char key, int x, int y)
{
	m_keystate[key] = false;

	for (auto callback : m_key_release_callbacks)
	{
		  callback.second.first(key, callback.second.second);
	}
}

/// Call callbacks registered using KeyBoard::registerSpecialKeyPressCallback() 
void KeyBoard::onSpecialKeyPress(int key, int x, int y)
{
	if (key > 255) return;
	m_keystate_special[key] = true;

	for (auto callback : m_special_key_press_callbacks)
	{
		  callback.second.first(key, callback.second.second);
	}
}

/// Call callbacks registered using KeyBoard::registerSpecialKeyReleaseCallback() 
void KeyBoard::onSpecialKeyRelease(int key, int x, int y)
{
	if (key > 255) return;
	m_keystate_special[key] = false;

	for (auto callback : m_special_key_release_callbacks)
	{
		  callback.second.first(key, callback.second.second);
	}
}
