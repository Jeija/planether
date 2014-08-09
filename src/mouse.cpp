#ifdef PLANETHER_WINDOWS
	#include <windows.h>
#endif

#include <iostream>
#include "gllibs.hpp"

#include "gamevars.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "player.hpp"
#include "mouse.hpp"
#include "game.hpp"
#include "math.h"

/// Preparation of mouse, only required on MS Windows
Mouse::Mouse() :
m_ignore(false)
{
#ifdef PLANETHER_WINDOWS
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	GetCursorPos(&m_centerpos);
#endif
}

void on_mouse_click_default(int button, int state, int x, int y, void *unused)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		game->getCamera()->setCaptureMouse(true);
	}
}

/**
 * \brief Notify the mouse class of a move event
 * \param x The new x position of the mouse
 * \param y The new y position of the mouse
 */
void Mouse::onMouseMove(int x, int y)
{
	if (m_ignore)
	{
		m_ignore = !m_ignore;
		return;
	}

	if (x == glutGet(GLUT_WINDOW_WIDTH)/2 and y == glutGet(GLUT_WINDOW_HEIGHT)/2) return;
	if (game->getTportOverlay()) return;
	if (!game->getCamera()->getCaptureMouse()) return;

#ifndef PLANETHER_WINDOWS
	/*
		Ignore movements that are too fast, they're just
		errors created e.g. when switching to fullscreen
		and otherwise unpredictable
	*/

	if (abs(x - glutGet(GLUT_WINDOW_WIDTH ) / 2) > 150) return;
	if (abs(y - glutGet(GLUT_WINDOW_HEIGHT) / 2) > 150) return;
#endif

	for (auto callback : m_move_callbacks)
		callback.first(x, y, callback.second);

	// Put mouse pointer back
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);

#ifdef PLANETHER_WINDOWS
	GetCursorPos(&m_centerpos);
#endif
}

/**
 * \brief Notify the mouse class of a click event
 * \param button The ID of the pressed button
 * \param state The state (pressed / released) of the mouse button
 * \param x The x position of the mouse at that moment
 * \param y The y position of the mouse at that moment
 */
void Mouse::onMouseClick(int button, int state, int x, int y)
{
	if (game->getTportOverlay()) return;

	// Click callbacks
	for (auto callback : m_click_callbacks)
		callback.first(button, state, x, y, callback.second);
}

/**
 * \brief Registers a callback function to be called when the mouse is moved
 * \param Callback function to be called
 * \param param Any pointer that will be passed to the callback function
 *
 * The pointer can be used to store a reference to a class so
 * that a member function can be called via a wrapper.
 */
void Mouse::registerMoveCallback(void (*callback)(int x, int y, void *param), void *param)
{
	m_move_callbacks.push_back(std::make_pair(callback, param));
}

/**
 * \brief Registers a callback function to be called when a mouse button is clicked
 * \param Callback function to be called
 * \param param Any pointer that is will be to the callback function
 *
 * The pointer can be used to store a reference to a class so
 * that a member function can be called via a wrapper.
 */
void Mouse::registerClickCallback(void (*callback)(int button, int state, int x, int y, void *param),
	void *param)
{
	m_click_callbacks.push_back(std::make_pair(callback, param));
};

#ifdef PLANETHER_WINDOWS
void Mouse::handleMicrosoftWindows()
{
	// Get mouse movement using windows.h
	POINT cursor;
	GetCursorPos(&cursor);
	POINT mousemove;
	mousemove.x = cursor.x - m_centerpos.x;
	mousemove.y = cursor.y - m_centerpos.y;

	if (game->getCamera()->getCaptureMouse() && mousemove.x != 0 && mousemove.y != 0)
	{
		onMouseMove(glutGet(GLUT_WINDOW_WIDTH)/2 + mousemove.x,
			glutGet(GLUT_WINDOW_HEIGHT)/2 + mousemove.y);
	}
}
#endif
