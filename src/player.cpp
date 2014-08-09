#include "spaceship.hpp"
#include "quatutil.hpp"
#include "keyboard.hpp"
#include "gamevars.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "mouse.hpp"
#include "debug.hpp"
#include "util.hpp"
#include "game.hpp"
#include "math.h"

/*
	Class Player
*/

/**
 * \brief The Player class handles all the movement of the camera
 * \param pos The position where the player is initially placed
 * \param angles The initial look angles of the player
 *
 * The player class can be modified by all the input devices such as
 * mouse and keyboard and then calculates the resulting vectors
 * using glm::quat (quaternions). That way, there are no issues like
 * gimbal lock and such.
 */
Player::Player(SimpleVec3d pos, SimpleAngles angles) :
m_look(anglesToQuat(angles))
{
	m_pos = pos;
	m_velocity = SimpleVec3d();

	// Keyboard functions
	keyboard->registerCallback(onKeyCallback_wrapper, this);
	keyboard->registerKeyPressCallback(onKeyPress_wrapper, this);
	keyboard->registerKeyReleaseCallback(onKeyRelease_wrapper, this);

	// Mouse functions
	mouse->registerMoveCallback(onMouseMove_wrapper, this);
	mouse->registerClickCallback(onMouseClick_wrapper, this);
}

/**
 * \brief Destructs the player
 *
 * This actually doesn't delete anything but print the destructor message
 */
Player::~Player()
{
	std::cout<<"~Player"<<std::endl;
}

/**
 * \brief Get the look angles of the player
 * \return The player's look angles
 * The value is calculated by making an axis out of the look quaternions
 * and then making the SimpleAngles out of the axis (vector).
 */
SimpleAngles Player::getAngles()
{
	return SimpleAngles(getLookAxis());
}

/**
 * \brief Set the look angles of the player
 * \param look The look angles the player should have
 *
 * The player calculates its look quaternion by making a vector
 * out of the angles and then retrieving the quaternion from
 * the vector.
 */
void Player::setLookAngles(SimpleAngles look)
{
	m_look = anglesToQuat(look);
	m_look = glm::normalize(m_look);

	// Recalculate look, up, right axis
	quatToMounting(m_look, &m_lookaxis, &m_upaxis, &m_rightaxis);
}

/**
 * \brief Set the player's look quaternion
 * \param look The look quaternion to be set
 *
 * Sets the player's look quaternion and recalculates look-, up- and rightaxis
 */
void Player::setLookQuat(glm::quat look)
{
	m_look = look;

	// Recalculate look, up, right axis
	quatToMounting(m_look, &m_lookaxis, &m_upaxis, &m_rightaxis);
}

/**
 * \brief Rotates the player's view by a yaw and a pitch value
 * \param xmov The yaw rotation based on the x-movement of the mouse
 * \param ymov The pitch rotation based on the y-movement of the mouse
 *
 * To be called e.g. by the mouse callback function that rotates the players look direction.
 */
void Player::rotView(float xmov, float ymov)
{
	SimpleVec3d newlookdir(SimpleAngles(xmov, ymov, 0));
	SimpleVec3d defLookDir(0, 0, -1);
	m_look *= RotationBetweenVectors(defLookDir, newlookdir);
	m_look = glm::normalize(m_look);

	// Recalculate look, up, right axis
	quatToMounting(m_look, &m_lookaxis, &m_upaxis, &m_rightaxis);
}

/**
 * \brief Add to the player's view rotation speed using a yaw and a pitch value
 * \param xmov The yaw rotation speed in radians per second
 * \param ymov The pitch rotation speed in radians per second
 *
 * To be called e.g. by the Player's keyboard callback function for the numpad keys.
 */
void Player::addRotSpeed(float xmov, float ymov)
{
	SimpleVec3d newlookdir(SimpleAngles(xmov, ymov, 0));
	SimpleVec3d defLookDir(0, 0, -1);
	m_rotvel *= RotationBetweenVectors(defLookDir, newlookdir);
}

/**
 * \brief Rotates the player's view roll by a roll value
 * \param roll The roll value to rotate the player's look quaternion by
 *
 * To be called e.g. by the function handling the mouse wheel.
 */
void Player::rotRoll(float r)
{
	// Create the Quaternion from upvector and upvector with roll
	glm::quat rollquat = RotationBetweenVectors(
		SimpleVec3d(0, 1, 0),
		SimpleVec3d(SimpleAngles(PI/2, PI/2 - r, 0))
	);

	// Apply rotation Quaternion (rollquat) to look Quaternion
	m_look *= rollquat;
	m_look = glm::normalize(m_look);

	// Recalculate look, up, right axis
	quatToMounting(m_look, &m_lookaxis, &m_upaxis, &m_rightaxis);
}

/**
 * \brief Add a given value to the player's current roll rotation speed
 * \param r The value to be added
 */
void Player::accRollSpeed(float r)
{
	// Create the Quaternion from upvector and upvector with roll
	glm::quat rollquat = RotationBetweenVectors(
		SimpleVec3d(0, 1, 0),
		SimpleVec3d(SimpleAngles(PI/2, PI/2 - r, 0))
	);

	// Apply rotation Quaternion (rollquat) to rotvel Quaternion
	m_rotvel *= rollquat;
	m_rotvel = glm::normalize(m_rotvel);
}

/**
 * \brief Set the player's current roll rotation speed to a given value
 * \param r The value to be set
 */
void Player::setRollSpeed(float r)
{
	// Set roll component of rotation velocity quaternion to zero
	glm::quat rotroll_comp = m_rotvel;
	rotroll_comp.x = 0; // Roll component of
	rotroll_comp.y = 0; // of m_rotvel quat
	rotroll_comp = glm::normalize(rotroll_comp);
	m_rotvel *= conjugateQuat(rotroll_comp);

	// Set roll component to new value
	accRollSpeed(r);
}

void Player::step(float dtime)
{
	m_velocity -= m_movevel_toremove;
	m_movevel_toremove = rotateVecByQuat(m_movevel / game->getGameSpeed(), m_look);
	m_velocity += m_movevel_toremove;

	physicalMove(dtime);

	// Apply rotation speed
	if (m_rotvel.w != 1.0) // only if there even is a rotation (no unit quaternion)
	{
		m_look = glm::mix(m_look, m_look  * m_rotvel, dtime / game->getGameSpeed() * 10.0f);
		quatToMounting(m_look, &m_lookaxis, &m_upaxis, &m_rightaxis);
		m_look = glm::normalize(m_look);
	}
}

/**
 * \brief Wrapper function that calls Player::onKeyPress
 */
void Player::onKeyPress_wrapper (unsigned char key, void *self)
{
	((Player *)self)->onKeyAction(key, true);
}

/**
 * \brief Wrapper function that calls Player::onKeyRelease
 */
void Player::onKeyRelease_wrapper (unsigned char key, void *self)
{
	((Player *)self)->onKeyAction(key, false);
}

/**
 * \brief Performs the player's actions when a key is pressed or released
 */
void Player::onKeyAction(unsigned char key, bool pressed)
{
	if (game->getTportOverlay()) return; // keypresses used by TeleportWindow

	// if not yet used, assume that the key was not pressed
	if (m_keystate.find(key) == m_keystate.end()) m_keystate[key] = false;
	if (m_keystate[(uint8_t)key] == pressed) return; // nothing changed actually
	m_keystate[(uint8_t)key] = pressed;

	int8_t d = (pressed ? 1 : -1);

	double move_speed = config->getDouble("move_speed", 8000.0) * USC;
	// Velocity
	if (key == 'w')
		m_movevel += SimpleVec3d(0, 0, -move_speed * d);

	if (key == 'a')
		m_movevel += SimpleVec3d(-move_speed * d, 0, 0);

	if (key == 's')
		m_movevel += SimpleVec3d(0, 0, move_speed * d);

	if (key == 'd')
		m_movevel += SimpleVec3d(move_speed * d, 0, 0);

	if (key == 'e')
		m_movevel += SimpleVec3d(0, -move_speed * d, 0);

	if (key == ' ')
		m_movevel += SimpleVec3d(0, move_speed * d, 0);
}

/**
 * \brief Wrapper function that calls Player::onKeyCallback
 */
void Player::onKeyCallback_wrapper (bool keystate[255], bool ks_special[255], float dtime, void *self)
{
	((Player *)self)->onKeyCallback(keystate, ks_special, dtime);
}

/**
 * \brief Callback that performs NumPad Rotation
 */
void Player::onKeyCallback(bool keystate[255], bool ks_special[255], float dtime)
{
	float d = dtime / game->getGameSpeed(); // "real" dtime
	float keyb_rotspeed = config->getDouble("keyboard_rotate_speed", PI/6);

	// Rotation
	if (keystate[(uint8_t)'4'])
		rotView(-keyb_rotspeed * d, 0);

	if (keystate[(uint8_t)'6'])
		rotView(keyb_rotspeed * d, 0);

	if (keystate[(uint8_t)'2'])
		rotView(0, -keyb_rotspeed * d);

	if (keystate[(uint8_t)'8'])
		rotView(0, keyb_rotspeed * d);

	if (keystate[(uint8_t)'7'])
		rotRoll(-keyb_rotspeed * d);

	if (keystate[(uint8_t)'9'])
		rotRoll(keyb_rotspeed * d);
}

/**
 * \brief Wrapper function that calls Player::onMouseClick
 */
void Player::onMouseClick_wrapper (int button, int state, int x, int y, void *self)
{
	((Player *)self)->onMouseClick(button, state, x, y);
}

/**
 * \brief Performs the player's actions when a mouse key is pressed or released
 * \param button The mouse button that was changed
 * \param state The new state of the mouse button
 * \param x The mouse position x at that moment (unused)
 * \param y The mouse position y at that moment (unused)
 * These are rolling or with mosuewheel and special keys movement.
 */
void Player::onMouseClick (int button, int state, int x, int y)
{
	// Only perform an action if state of the button has changed
	if (m_mouse.find(button) == m_mouse.end()) m_mouse[button] = GLUT_UP;
	if (button > 19 || m_mouse[button] == state) return;
	m_mouse[button] = state;

	// Reset any movement when middle button is pressed
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		m_movevel = SimpleVec3d();

	// Get config
	double move_speed = config->getDouble("move_speed", 8000.0) * USC;
	double mousewheel_speed = config->getDouble("mousewheel_move_speed", 1500.0) * USC;

	// Move with mouse wheel, right/left mouse wheel, side buttons
	if (button == 3) // wheel up
		moveBy(m_lookaxis * mousewheel_speed);

	if (button == 4) // wheel down
		moveBy(m_lookaxis * -mousewheel_speed);

	if (button == 5) // right/left button
		m_movevel += SimpleVec3d(-move_speed, 0, 0);

	if (button == 6) // right/left button
		m_movevel += SimpleVec3d(move_speed, 0, 0);

	int8_t d = (state == GLUT_DOWN ? 1 : -1);
	if (button == 7) // side buttons
		m_movevel += SimpleVec3d(0, -move_speed * d, 0);

	if (button == 8) // side buttons
		m_movevel += SimpleVec3d(0, move_speed * d, 0);

	// rolling with mouse buttons
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		accRollSpeed(-MOUSE_BUTTON_ROLLSPEED);
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		accRollSpeed(MOUSE_BUTTON_ROLLSPEED);

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		accRollSpeed(MOUSE_BUTTON_ROLLSPEED);
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		accRollSpeed(-MOUSE_BUTTON_ROLLSPEED);
}

/**
 * \brief Wrapper function that calls onMouseMove
 */
void Player::onMouseMove_wrapper(int x, int y, void *self)
{
	((Player *)self)->onMouseMove(x, y);
}

/**
 * \brief Rotates the Player's view when the mouse is moved
 */
void Player::onMouseMove(int x, int y)
{
	if (!game->getCamera()->getCaptureMouse()) return;

	float ymove = -(y - glutGet(GLUT_WINDOW_HEIGHT) / 2) * MOUSE_SENSITIVITY; // vertical
	float xmove =  (x - glutGet(GLUT_WINDOW_WIDTH ) / 2) * MOUSE_SENSITIVITY; // horizontal

	game->getPlayer()->rotView(xmove, ymove);
}
