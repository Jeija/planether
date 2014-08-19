#include <sstream>
#include <string>

#include "navigation.hpp"
#include "spaceship.hpp"
#include "gamevars.hpp"
#include "keyboard.hpp"
#include "drawutil.hpp"
#include "teleport.hpp"
#include "objects.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "gllibs.hpp"
#include "game.hpp"
#include "hud.hpp"

/*
	CrossHair
*/

CrossHair::CrossHair() :
m_hidden(false)
{
	m_img = new Image2d("crosshair.png");
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	keyboard->registerSpecialKeyPressCallback(onSpecialKeyPress_wrapper, this);
}

CrossHair::~CrossHair()
{
	delete m_img;
}

void CrossHair::reshape(int width, int height)
{
	float aspectratio =  width * 1.0 / height;
	float crosshair_w = 0.08/aspectratio;
	float crosshair_h = 0.08;

	m_img->setBounds(-crosshair_w/2, -crosshair_h/2,
			  crosshair_w  ,  crosshair_h);
}

void CrossHair::render(int window_w, int window_h)
{
	if (m_hidden) return;

	if (game->getCamera()->getCaptureMouse())
		m_img->render();
}

/**
 * \brief Wrapper function that calls onSpecialKeyPress
 */
void CrossHair::onSpecialKeyPress_wrapper(int key, void *self)
{
	((CrossHair*) self)->onSpecialKeyPress(key);
}

/**
 * \brief Performs actions to the CrossHair when a key is pressed
 * Toggles hidden value of the CrossHair when F1 is pressed.
 */
void CrossHair::onSpecialKeyPress(int key)
{
	if (key == GLUT_KEY_F1)
		m_hidden = !m_hidden;
}


/*
	PlanetLocator
*/

PlanetLocator::PlanetLocator(Game *game):
	m_hidden(false),
	m_game(game)
{
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	keyboard->registerSpecialKeyPressCallback(onSpecialKeyPress_wrapper, this);
}

void PlanetLocator::reshape(int width, int height)
{
	
}

#include <algorithm>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include "environment.hpp"
#include "player.hpp"
#include "teleport.hpp"
#include "spaceship.hpp"
#include "navigation.hpp"

void PlanetLocator::render(int window_w, int window_h)
{
	if (m_hidden) return;

	glColor3f(1.0f, 1.0f, 1.0f);
	float vAmt = (5.0f / (float)window_h);
	float hAmt = (5.0f / (float)window_w);

	TeleportTarget *target = game->getSpaceship()->getNavigator()->getTarget();
	for (std::map<std::string, PlanetLocator::v2>::const_iterator it = locations.begin();
			it != locations.end();
			++it)
	{
		if (target && target->getTeleportName() == it->first)
			glColor3f(0.0f, 1.0f, 0.0f);

		glRectf(it->second.X - hAmt, it->second.Y + vAmt, it->second.X + hAmt, it->second.Y - vAmt);

		glRasterPos2f(it->second.X + 3*hAmt, it->second.Y);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *)it->first.c_str());

		if (target && target->getTeleportName() == it->first)
			glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void PlanetLocator::whileWorldMatrix(int window_w, int window_h)
{
	Player *player = m_game->getPlayer();
	WorldEnvironment *world_env = m_game->getWorldEnv();
	std::vector<WorldObject*> worldobjects = world_env->getObjects();
	locations.clear();
	for (auto obj : worldobjects)
	{
		std::string name = "";
		{
			TeleportTarget *st = dynamic_cast<TeleportTarget*>(obj);
			if (!st)
			{
				continue;
			}
			else
			{
				name = st->getTeleportName();				
			}
		}

		double centerX, centerY, centerZ = 0;
		GLdouble modelviewMatrix[16], projectionMatrix[16];
		GLint viewport[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
		glGetIntegerv(GL_VIEWPORT, viewport);
		SimpleVec3d ppos = player->getPos();
		gluProject(obj->getPos().x - ppos.x, obj->getPos().y - ppos.y, obj->getPos().z - ppos.z,
				modelviewMatrix, projectionMatrix, viewport, &centerX, &centerY, &centerZ);		
		float x = 2.0f * ((float)centerX / (float)window_w) - 1;
		float y = 2.0f * ((float)centerY / (float)window_h) - 1;
		if (centerZ < 1)
			locations[name.c_str()] = PlanetLocator::v2(x, y);
	}
}

/**
 * \brief Wrapper function that calls onSpecialKeyPress
 */
void PlanetLocator::onSpecialKeyPress_wrapper(int key, void *self)
{
	((PlanetLocator*) self)->onSpecialKeyPress(key);
}

/**
 * \brief Performs actions to the CrossHair when a key is pressed
 * Toggles hidden value of the CrossHair when F1 is pressed.
 */
void PlanetLocator::onSpecialKeyPress(int key)
{
	std::cerr << "keypress" << std::endl;
	int window_w = glutGet(GLUT_WINDOW_WIDTH);
	int window_h = glutGet(GLUT_WINDOW_HEIGHT);
	if (key == GLUT_KEY_F1)
		m_hidden = !m_hidden;
	if (key == GLUT_KEY_F4)
	{
		std::cerr << "f4" << std::endl;
		float vAmt = 20.0f / (float)window_h;
		float hAmt = 20.0f / (float)window_w;

		for (std::map<std::string, PlanetLocator::v2>::const_iterator it = locations.begin();
				it != locations.end();
				++it)
		{
			if (it->second.X > -hAmt && it->second.X < hAmt && it->second.Y > -vAmt && it->second.Y < vAmt)
			{	
				std::cerr << "at" << std::endl;			
				WorldEnvironment *world_env = m_game->getWorldEnv();
				std::vector<WorldObject*> worldobjects = world_env->getObjects();
				for (auto obj : worldobjects)
				{
					TeleportTarget *st = dynamic_cast<TeleportTarget*>(obj);
					if (st && st->getTeleportName() == it->first)
					{
						std::cerr << "nav" << std::endl;
						game->getSpaceship()->getNavigator()->setTarget(st);	
						game->getSpaceship()->getNavigator()->start();
						return;				
					}
				}
				return;
			}
		}
	}
}

/*
	PhysicsInformation
*/

PhysicsInformation::PhysicsInformation() :
m_hidden(false)
{
	keyboard->registerSpecialKeyPressCallback(onSpecialKeyPress_wrapper, this);
}

void PhysicsInformation::render(int window_w, int window_h)
{
	if (m_hidden) return;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate size of 1 pixel
	float ps_w = 2. / window_w; // pixelsize x-direction
	float ps_h = 2. / window_h; // pixelsize y-direction

	// Timelapse
	std::ostringstream conversion_tl;
	conversion_tl<<game->getGameSpeed();
	std::string gamespeed_str = conversion_tl.str();
	std::string timelapse_text = "Timelapse: " + gamespeed_str + "x";

	glRasterPos2f(-1 + 5 * ps_w, 1 - 23 * ps_h);

	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)timelapse_text.c_str());

	// Camera type
	std::string camtype;
	switch (game->getSpaceship()->getCamBind())
	{
		case CAMERA_BOUND:
			camtype = "bound";
			break;
		case CAMERA_RELATIVE_ROT:
			camtype = "relative with rotation";
			break;
		case CAMERA_RELATIVE:
			camtype = "relative";
			break;
		case CAMERA_FREE:
			camtype = "free";
			break;
	}
	std::string camtype_text = "Camera: " + camtype;

	glRasterPos2f(-1 + 5 * ps_w, 1 - 46 * ps_h);

	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)camtype_text.c_str());

	// Ship Speed
	float shipvel = getVectorLength(game->getSpaceship()->getVelocity()) / USC;
	std::string speedtext = "Speed: " + std::to_string(shipvel) + " km/s";

	glRasterPos2f(-1 + 5 * ps_w, 1 - 69 * ps_h);

	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)speedtext.c_str());

	if (shipvel > SPEED_OF_LIGHT)
	{
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glRasterPos2f(1 - 150 * ps_w, 1 - 23 * ps_h);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)"SPEED OF LIGHT!");
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Gravity Acceleration
	SimpleVec3d gravacc_v = game->getSpaceship()->getGravityAcc();
	float gravacc = getVectorLength(gravacc_v) * 1000 / USC; // / 1000 for km/s² to m/s²
	std::string gravacc_text = "Acceleration: " + std::to_string(gravacc) + " m/s ";
	gravacc_text.push_back(0xB2); // ² character

	glRasterPos2f(-1 + 5 * ps_w, 1 - 92 * ps_h);

	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)gravacc_text.c_str());
}

/**
 * \brief Wrapper function that calls onSpecialKeyPress
 */
void PhysicsInformation::onSpecialKeyPress_wrapper(int key, void *self)
{
	((PhysicsInformation*) self)->onSpecialKeyPress(key);
}

/**
 * \brief Performs actions to the PhysicsInformation when a key is pressed
 * Toggles hidden value of the PhysicsInformation when F2 is pressed.
 */
void PhysicsInformation::onSpecialKeyPress(int key)
{
	if (key == GLUT_KEY_F2)
		m_hidden = !m_hidden;
}

/*****************************
***	TeleportWindow     ***
******************************/


/*
	TeleportAction
*/
TeleportWindowAction::TeleportWindowAction(std::string description, void (*action)(TeleportTarget*)) :
m_description(description),
m_action(action),
m_action_error(nullptr)
{
}

void TeleportWindowAction::execute(TeleportTarget *target)
{
	m_action(target);
}

void TeleportWindowAction::executeError()
{
	if (m_action_error != nullptr)
		m_action_error();
}


TeleportWindow::TeleportWindow() :
m_target(nullptr),
m_preview_time(0),
m_action_selected(0)
{
	m_keyb_callb_id = keyboard->registerKeyPressCallback(onKeyPress_wrapper, this);
	m_spec_keyb_callb_id = keyboard->registerSpecialKeyPressCallback
		(onSpecialKeyPress_wrapper, this);
	game->setTportOverlay(true);

	m_audio = new AudioNode();
	m_audio->addFile("beep_open.ogg");
	m_audio->play();
	game->getAudioEnv()->bindNode(m_audio);

	m_actions.push_back(TeleportWindowAction("Teleport to:", TeleportWindow::action_teleportTo));
	m_actions.push_back(TeleportWindowAction("Teleport Spaceship to:",
		TeleportWindow::action_teleportSpaceship));

	TeleportWindowAction nav = TeleportWindowAction("Navigate to:",
		TeleportWindow::action_navigateTo);
	nav.setErrorAction(TeleportWindow::action_navigateToError);
	m_actions.push_back(nav);

	m_available_targets = getAllTeleportTargets();
}

TeleportWindow::~TeleportWindow()
{
	// Deleting m_audio will be handled by the AudioEnvironment as m_audio is bound to it
	// and setDeleteOnFinish is set to true
	m_audio->setDeleteOnFinish(true);

	keyboard->unRegister(m_keyb_callb_id);
	keyboard->unRegister(m_spec_keyb_callb_id);
	game->setTportOverlay(false);
}

void TeleportWindow::step(float dtime)
{
	m_preview_time += dtime;
}

void TeleportWindow::render(int window_w, int window_h)
{
	// Calculate size of 1 pixel
	float ps_w = 2. / window_w; // pixelsize x-direction
	float ps_h = 2. / window_h; // pixelsize y-direction
	float apsr = 1.*window_w / window_h; // aspectatio

	// Whole Window
	glColor3f(0.0, 0.0, 0.1);
	glBegin(GL_QUADS);
	{
		glVertex3f(-0.9, -0.9, -1.0);
		glVertex3f(-0.9,  0.9, -1.0);
		glVertex3f( 0.9,  0.9, -1.0);
		glVertex3f( 0.9, -0.9, -1.0);
	}
	glEnd();

	// Preview Widget
	glColor3f (0.1, 0.1, 0.1);
	glBegin(GL_QUADS);
	{
		glVertex3f(-0.45/apsr, -0.45, -1.0);
		glVertex3f(-0.45/apsr,  0.45, -1.0);
		glVertex3f( 0.45/apsr,  0.45, -1.0);
		glVertex3f( 0.45/apsr, -0.45, -1.0);
	}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.9 + 4 * ps_w, -0.9 + 4 * ps_h);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12,
		(unsigned char *)"Use arrow keys up and down to select the desired action.\
		Use arrow keys right and left to select your destination or type its name.");

	glColor3f(1, 1, 1);
	unsigned char *actionstring = (unsigned char *)
		m_actions.at(m_action_selected).getDescription().c_str();
	float actionstring_len = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, actionstring);

	glRasterPos2f(0 - actionstring_len / 2 * ps_w, 1 - 70 * ps_h);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, actionstring);

	float offset_l = m_destination.length() * 4.5;
	glRasterPos2f(0 - offset_l * ps_w, 1 - 93 * ps_h);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)m_destination.c_str());

	glColor3f(1, 1, 1);
	// Render preview of TeleportTarget
	glPushAttrib(GL_ENABLE_BIT);
	{
		glEnable (GL_DEPTH);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_BLEND);

		glScalef(1.0f / apsr, 1.0f, 1.0f);

		glTranslatef(0.0f, 0.0f, -1.0f);
		if (m_target)
			m_target->renderPreview(m_preview_time, 0.4f);
		else
		{
			// Draw Wire Sphere
			glLineWidth(2.0);
			glPushMatrix();
			{
				glRotatef(m_preview_time * TELEPORT_PREVIEW_ROTSPEED, 0, 1, 0);
				glRotatef(90, 1, 0, 0);
				glutWireSphere(0.4, 20, 20);
			}
			glPopMatrix();

			// Draw red question mark
			glPushMatrix();
			{
				glColor3f(1.0f, 0.0f, 0.0f);
				glLineWidth(5.0);
				float textsize = 0.5;
				float font_height = textsize / glutStrokeHeight(GLUT_STROKE_MONO_ROMAN);
				float font_width  = textsize / glutStrokeLength(GLUT_STROKE_MONO_ROMAN,
					(unsigned char *)"?");

				glTranslatef(0.0f, 0.0f, 1.0f);
				glRotatef(m_preview_time * TELEPORT_PREVIEW_ROTSPEED, 0, 1, 0);
				glTranslatef(-textsize / 2, -textsize / 4, 0.4f);
				glScalef(font_width, font_height, font_width);
				glutStrokeString(GLUT_STROKE_MONO_ROMAN, (unsigned char *)"?");
			}
		}
	}
	glPopAttrib();
}

void TeleportWindow::onKeyPress_wrapper (unsigned char key, void *self)
{
	((TeleportWindow *)self)->onKeyPress(key);
}

void TeleportWindow::onKeyPress (unsigned char key)
{
	switch (key)
	{
		case 0x1b: // Escape = 0x1b --> close window
			m_obsolete = true;
			break;

		case (unsigned char) 13: // enter key --> close window + teleport
		{
			m_obsolete = true;

			if(m_target != nullptr)
			{
				m_audio->addFile("beep_ok.ogg");
				m_actions.at(m_action_selected).execute(m_target);
			}
			else
			{
				m_audio->addFile("beep_error.ogg");
				m_actions.at(m_action_selected).executeError();
			}

			m_audio->play();

			break;
		}

		case (unsigned char)'\b': // backspace
			if (m_destination.length() > 0)
				m_destination.pop_back();
			break;

		default:
			m_destination += key;
			break;
	}

	// If target changes, reset time
	if (m_target != getTeleportTarget(m_destination)) m_preview_time = 0;
	m_target = getTeleportTarget(m_destination);
}

void TeleportWindow::onSpecialKeyPress_wrapper(int key, void *self)
{
	((TeleportWindow *)self)->onSpecialKeyPress(key);
}

void TeleportWindow::onSpecialKeyPress (int key)
{
	bool found = false; // for KEY_RIGHT and KEY_LEFT only
	switch(key)
	{
		case GLUT_KEY_UP:
			++m_action_selected;
			if (m_action_selected >= m_actions.size())
				m_action_selected = 0; // goto first element
			break;
		case GLUT_KEY_DOWN:
			if (m_action_selected == 0)
				m_action_selected = m_actions.size() - 1; // goto last element
			else
				--m_action_selected;
			break;
		case GLUT_KEY_RIGHT:
			// Choose next item from available TeleportTargets
			for (std::vector<std::string>::iterator it = m_available_targets.begin();
				it != m_available_targets.end(); ++it)
			{
				if ((*it) == m_destination && it != m_available_targets.end() - 1)
				{
					m_destination = (*(++it));
					found = true;
				}
			}

			if (!found)
				m_destination = m_available_targets.front();
			m_target = getTeleportTarget(m_destination);
			break;

		case GLUT_KEY_LEFT:
			// Choose next item from available TeleportTargets
			for (std::vector<std::string>::iterator it = m_available_targets.begin();
				it != m_available_targets.end(); ++it)
			{
				if ((*it) == m_destination && it != m_available_targets.begin())
				{
					m_destination = (*(--it));
					found = true;
				}
			}

			if (!found)
				m_destination = m_available_targets.back();
			m_target = getTeleportTarget(m_destination);
			break;
	}
}

void TeleportWindow::action_teleportTo(TeleportTarget *target)
{
	SimpleVec3d	pos = target->getTeleportPos();
	SimpleAngles	ang = target->getTeleportAngles();
	game->getPlayer()->setPos(pos);
	game->getPlayer()->setLookAngles(ang);
	game->getPlayer()->setVelocity(SimpleVec3d());
}

void TeleportWindow::action_teleportSpaceship(TeleportTarget *target)
{
	SimpleVec3d	pos = target->getTeleportPos();
	SimpleAngles	ang = target->getTeleportAngles();
	game->getSpaceship()->setPos(pos);
	game->getSpaceship()->setAngles(ang);
	game->getSpaceship()->setVelocity(SimpleVec3d());
	game->getSpaceship()->setVelQuat(glm::quat());
}

void TeleportWindow::action_navigateTo(TeleportTarget *target)
{
	game->getSpaceship()->getNavigator()->setTarget(target);
	game->getSpaceship()->getNavigator()->start();
}

void TeleportWindow::action_navigateToError()
{
	game->getSpaceship()->getNavigator()->stop();
}
