#include <algorithm>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "gllibs.hpp"

#include "environment.hpp"
#include "spaceship.hpp"
#include "gamevars.hpp"
#include "objects.hpp"
#include "config.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "debug.hpp"
#include "game.hpp"
#include "util.hpp"

/**
 * \brief The Camera renders objects on the screen based on the player's view
 * \param world_env The WorldEnvironment to associate with the camera
 * \param static_env The StaticEnvironment to associate with the camera
 * \param Player The Player whose position and look direction are to be used for rendering
 * \param shaderman The ShaderManager that the camera should store
 *
 * The camera stores references to all the objects to be rendered on the screen and creates
 * the matrices to render the objects at their given positions. The matrices are calculated
 * based on the information from the associated player. The camera also does some basic
 * sorting based on the positions of the associated objects.
 */
Camera::Camera(	WorldEnvironment *world_env, StaticEnvironment *static_env, Player *player,
		ShaderManager *shaderman, SkyBox *skybox) :
m_world_env(world_env),
m_static_env(static_env),
m_player(player),
m_shaderman(shaderman),
m_skybox(skybox),
m_framecounter(new FrameCounter()),
m_capture_mouse(true)
{
}

/**
 * Destroys the Camera
 */
Camera::~Camera()
{
	delete m_framecounter;
	m_framecounter = nullptr;

	std::cout<<"~Camera"<<std::endl;
}

/**
 * \brief Renders the current scene
 * 
 * Renders WorldEnvironment, StaticEnvironment and StaticWorldEnvironment (only the skybox belongs
 * to that, it is a WorldEnvironment that moves with the player)
 */
void Camera::render()
{
	int window_w = glutGet(GLUT_WINDOW_WIDTH);
	int window_h = glutGet(GLUT_WINDOW_HEIGHT);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*********************************
		Static World Matrix
	*********************************/
	if (!config->getBool("enable_anaglyph", false))
	{
		// Normal mode, render the scene without any color mask / offset
		beginStaticWorldMatrix(window_w, window_h);
		{
			m_skybox->render();
		}
		endStaticWorldMatrix();
	}
	else
	{
		// Render two scenes, one in red and the other in cyan

		// Left eye (red)
		beginStaticWorldMatrix(window_w, window_h, CAMERA_EYE_LEFT);
		{
			glColorMask(true, false, false, false);
			m_skybox->render();
		}
		endStaticWorldMatrix();

		// Right eye (cyan)
		beginStaticWorldMatrix(window_w, window_h, CAMERA_EYE_RIGHT);
		{
			glColorMask(false, true, true, false);
			m_skybox->render();
		}
		endStaticWorldMatrix();
	}

	/***************************
		World Matrix
	***************************/
	if (!config->getBool("enable_anaglyph", false))
	{
		// Normal mode, render the scene without any color mask / offset
		beginWorldMatrix(window_w, window_h);
		{
			renderWorldMatrix();
		}
		endWorldMatrix();
	}
	else
	{
		// Render two scenes, one in red and the other in cyan

		// Left eye (red)
		beginWorldMatrix(window_w, window_h, CAMERA_EYE_LEFT);
		{
			glColorMask(true, false, false, false);
			renderWorldMatrix();
		}
		endWorldMatrix();
		glClear(GL_DEPTH_BUFFER_BIT) ;
	
		// Right eye (cyan)
		beginWorldMatrix(window_w, window_h, CAMERA_EYE_RIGHT);
		{
			glColorMask(false, true, true, false);
			renderWorldMatrix();
		}
		endWorldMatrix();

		glColorMask(true, true, true, true);
	}

	/*****************************
		Static Matrix
	******************************/
	beginStaticMatrix(window_w, window_h);
	{
		for (auto obj : m_static_env->getObjects())
		{
			obj->render(window_w, window_h);

			resetMaterial();
			m_shaderman->resetShader();
		}
	}
	endStaticMatrix();

	glFlush();
	glutSwapBuffers();

	m_framecounter->dispFrame();
}

/*
	Local Utilities:
*/

/**
 * \brief Prepares the OpenGL matrices for the WorldEnvironment
 * \param window_w The current window width to be used for the viewport
 * \param window_h The current window height to be used for the viewport
 * \param camera_eye The eye to render the scene for (CAMERA_EYE_CENTER as default value)
 *
 * Calculates the perspective and creates Modelview and Projection matrices. This
 * also enables all the OpenGL features such ass depth testing and lighting.
 */
void Camera::beginWorldMatrix(int window_w, int window_h, camera_eye eye)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, window_w, window_h);
	gluPerspective(45, window_w * 1.0 / window_h, USC * 100, BACKPLANE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable (GL_DEPTH		);
	glEnable (GL_DEPTH_TEST		);
	glEnable (GL_CULL_FACE		);
	glEnable (GL_BLEND		);
	glEnable (GL_LIGHTING		);
	glEnable (GL_SMOOTH		);
	glEnable (GL_NORMALIZE		);
	glEnable (GL_COLOR_MATERIAL	);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	SimpleVec3d lookaxis = m_player->getLookAxis() * 10.0;
	SimpleVec3d upaxis = m_player->getUpAxis();

	// If an eye is selected (not center), calculate a vector to move the camera by
	float ofs = config->getDouble("anaglyph_eyedist", 0.2) / 2.0 * USC * eye;
	SimpleVec3d offset = m_player->getRightAxis() * ofs;
	SimpleVec3d lookat = lookaxis;

	// The camera's position is 0, 0, 0 as we do not move the camera (apart from the little
	// translation it has when using anaglyph mode, by the value of cammoffset), but
	// the world around it. That way, the camera won't reach places that
	// have coordinates that are too high to store in a float 
	gluLookAt(offset.x, offset.y, offset.z,
		  lookat.x, lookat.y, lookat.z, 
		  upaxis.x, upaxis.y, upaxis.z);

	glRotatef(ofs * config->getDouble("anaglyph_intensity", 250), 0, 1, 0);
}

/**
 * \brief Disables features required by the WorldEnvironment
 *
 * Disables features such as depth testing and lighting, to be called after the WorldEnvironment
 * has been drawn.
 */
void Camera::endWorldMatrix()
{
	glDisable (GL_DEPTH		);
	glDisable (GL_DEPTH_TEST	);
	glDisable (GL_CULL_FACE		);
	glDisable (GL_BLEND		);
	glDisable (GL_LIGHTING		);
	glDisable (GL_SMOOTH		);
	glDisable (GL_COLOR_MATERIAL	);
}

/**
 * \brief Renders the WorldEnvironment
 *
 * Iterates through all objects in the WorldEnvironment, translates them and renders them.
 * This is a seperate function as it has to be called twice in Anaglyph mode.
 */
void Camera::renderWorldMatrix()
{
	std::vector<WorldObject*> worldobjects = m_world_env->getObjects();
	std::sort(worldobjects.begin(), worldobjects.end(), compareDistances);

	// Lighting enable
	for (auto obj : worldobjects)
		obj->getLightSpec().render((obj->getPos() - m_player->getPos()));

	// Render objects
	for (auto obj : worldobjects)
	{
		glPushMatrix();
		{
			(obj->getPos() - m_player->getPos()).translate();
			obj->render();

			resetMaterial();
			m_shaderman->resetShader();
		}
		glPopMatrix();
	}

	// Lighting disable
	for (auto obj : worldobjects)
		obj->getLightSpec().disable();
}


/**
 * \brief Prepares the OpenGL matrices for the StaticWorldEnvironment (Skybox)
 * \param window_w The current window width to be used for the viewport
 * \param window_h The current window height to be used for the viewport
 * \param camoffset Movement to the left / right of camera for anaglyph view (optional)
 *
 * Calculates the perspective and creates Modelview and Projection matrices. This
 * also enables all the OpenGL features such ass depth testing and lighting. This
 * function is very similar to beginWorldMatrix only that this take the players
 * position into account, but these environment is render around the player despite
 * its movement or position.
 */
void Camera::beginStaticWorldMatrix(int window_w, int window_h, camera_eye eye)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, window_w, window_h);
	gluPerspective(45, window_w * 1.0 / window_h, USC*0.00001, BACKPLANE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable (GL_DEPTH		);
	glEnable (GL_DEPTH_TEST		);
	glEnable (GL_CULL_FACE		);
	glEnable (GL_BLEND		);
	glEnable (GL_LIGHTING		);
	glEnable (GL_SMOOTH		);
	glEnable (GL_NORMALIZE		);
	glEnable (GL_COLOR_MATERIAL	);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	SimpleVec3d lookdir = m_player->getLookAxis();
	SimpleVec3d upaxis = m_player->getUpAxis();

	// If an eye is selected (not center), calculate a vector to move the camera by
	float ofs = config->getDouble("anaglyph_skybox_depth", 0.2) * USC * eye;
	SimpleVec3d offset = m_player->getRightAxis() * ofs;;
	SimpleVec3d lookat = lookdir + offset;

	gluLookAt(offset.x, offset.y, offset.z,
		  lookat.x, lookat.y, lookat.z, 
		  upaxis.x,  upaxis.y,  upaxis.z);
}

/**
 * \brief Disables features required by the StaticWorldEnvironment
 * Disables features such as depth testing and lighting, to be called after the WorldEnvironment
 * has been drawn.
 */
void Camera::endStaticWorldMatrix()
{
	glDisable (GL_DEPTH		);
	glDisable (GL_DEPTH_TEST	);
	glDisable (GL_CULL_FACE		);
	glDisable (GL_BLEND		);
	glDisable (GL_LIGHTING		);
	glDisable (GL_SMOOTH		);
	glDisable (GL_COLOR_MATERIAL	);
}

/**
 * \brief Prepares the OpenGL matrices for the StaticEnvironment
 * \param window_w The current window width to be used for the viewport
 * \param window_h The current window height to be used for the viewport
 *
 * Creates an orthogonal matrix that the StaticEnvironment uses.
 */
void Camera::beginStaticMatrix(int window_w, int window_h)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, window_w, 0, window_h, -10, 10);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * \brief Disables features required by the StaticEnvironment
 *
 * Pops the orthogonal matrix for the StaticEnvironment and disables
 * blending.
 */
void Camera::endStaticMatrix()
{
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


/**
 * \brief Compares distances of two WorldObjects
 * \param obj1 The first object to be compared
 * \param obj2 The second object to be compared
 * \return Returns true if the distance of obj1 to the player is greater than that of obj2
 *
 * This is a helper function that is used for the sorting algorithm of WorldObjects in render()
 */
bool Camera::compareDistances(WorldObject* obj1, WorldObject* obj2)
{
	float obj1_d = getVectorLength(game->getCamera()->getPlayer()->getPos() - obj1->getPos());
	float obj2_d = getVectorLength(game->getCamera()->getPlayer()->getPos() - obj2->getPos());

	return obj1_d > obj2_d;
}

/*
	Class FrameCounter
*/


/**
 * \brief Calculates the current fps of the program
 *
 * This helper class is uses by the camera to display the current fps (Frames per Second)
 * in the window title.
 */
FrameCounter::FrameCounter() :
m_fps(0),
m_last_rendertime(0)
{
	glutTimerFunc(FPS_UPDATETIME, FrameCounter::onTimerElapse, 0);
}

/**
 * \brief Update function for FrameCounter
 * \param ignored, it is required by glutTimerFunc
 *
 * This function regularly updates the current FPS display in the title bar
 * after FPS_UPDATETIME seconds. Wrapper function as glutTimerFunc takes
 * static / non-class members only.
 */
void FrameCounter::onTimerElapse(int ignore)
{
	FrameCounter *counter = game->getCamera()->getFrameCounter();
	if (counter != nullptr)
		counter->update_fps();

	glutTimerFunc(FPS_UPDATETIME, FrameCounter::onTimerElapse, 0);
}

/**
 * \brief Updates the FPS display in the window's title bar
 *
 * This function is to be called regularly by the FrameCounter::onTimerElapse wrapper
 */
void FrameCounter::update_fps(void) // Display FPS in title bar
{
	std::string title = std::string(APPLICATION_NAME) + " (FPS: " + std::to_string(m_fps) + ")";
	glutSetWindowTitle(title.c_str());
}

/**
 * \brief To be called by the camera when a frame is displayed.
 *
 * This function calculates the time between two frames and therefore must be called
 * each time a frame has been rendered.
*/
void FrameCounter::dispFrame(void) // Calculate FPS
{
	int time_per_frame = glutGet(GLUT_ELAPSED_TIME) - m_last_rendertime;
	m_fps = 1000 / time_per_frame;
	m_last_rendertime = glutGet(GLUT_ELAPSED_TIME);
}
