#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <string>

#include "util.h"

class StaticEnvironment;
class WorldEnvironment;
class ShaderManager;
class FrameCounter;
class WorldObject;
class SkyBox;
class Player;

/**
 * \brief Eye Position of the viewer
 *
 * CAMERA_EYE_CENTER is the default mode. In anaglyph mode,
 * the camera will render the scene for CAMERA_EYE_RIGHT or
 * CAMERA_EYE_LEFT.
 */
enum camera_eye
{
	CAMERA_EYE_CENTER = 0,	/** default mode, no anaglyph */
	CAMERA_EYE_RIGHT = 1,	/** Right eye */
	CAMERA_EYE_LEFT = -1	/** Left eye*/
};

/*
	Camera class - Does the rendering
*/

/// Renders the scene to the screen
class Camera
{
	public:
		Camera(	WorldEnvironment *world_env,
			StaticEnvironment *static_env,
			Player *player,
			ShaderManager *shaderman,
			SkyBox *skybox);

		~Camera();

		void render();

		/**
		 * \brief Sets the WorldEnvironment associated with the Camera
		 * \param The WorldEnvironment to associate with the Camera
		*/
		void setWorldEnv(WorldEnvironment *env)
			{ m_world_env = env; }

		/**
		 * \brief Sets the StaticEnvironment associated with the Camera
		 * \param The StaticEnvironment to associate with the Camera
		*/
		void setStaticEnv(StaticEnvironment *env)
			{ m_static_env = env; }

		/**
		 * \brief Retrieves the Player that is associated with the Camera
		 * \return The Player that is associated with the Camera
		 */
		Player *getPlayer()
			{ return m_player; }

		/**
		 * \brief Retrieve if the mouse is to be captured in the window
		 * \return True, if the mouse is to be captured, otherwise false
		 */
		bool getCaptureMouse()
			{ return m_capture_mouse; }

		/**
		 * \brief Set whether the mouse is to be captured in the window
		 * \parameter val True, if the mouse is to be captured, otherwise false
		 */
		void setCaptureMouse(bool val)
			{ m_capture_mouse = val; }

		/**
		 * \brief Retrieves the ShaderManager that is associated with the Camera
		 * \return The ShaderManager that is associated with the Camera
		 */
		ShaderManager *getShaderManager()
			{ return m_shaderman; }

		/**
		 * \brief Retrieves the SkyBox that is associated with the Camera
		 * \return The SkyBox that is associated with the Camera
		 */
		SkyBox *getSkyBox()
			{ return m_skybox; }

		/**
		 * \brief Retrieves the FrameCounter that is associated with the Camera
		 * \return The FrameCounter that is associated with the Camera
		 */
		FrameCounter *getFrameCounter()
			{ return m_framecounter; }

	private:
		void beginWorldMatrix(int window_w, int window_h,
			camera_eye eye = CAMERA_EYE_CENTER);
		void endWorldMatrix();
		void renderWorldMatrix();

		void beginStaticMatrix(int window_w, int window_h);
		void endStaticMatrix();

		void beginStaticWorldMatrix(int window_w, int window_h,
			camera_eye eye = CAMERA_EYE_CENTER);
		void endStaticWorldMatrix();

		static bool compareDistances(WorldObject* obj1, WorldObject* obj2);

		// Reference to the associated WorldEnvironment
		WorldEnvironment *m_world_env;

		// Reference to the associated StaticEnvironment
		StaticEnvironment *m_static_env;

		// Reference to the associated Player
		Player *m_player;

		// Reference to the associated ShaderManager
		ShaderManager *m_shaderman;

		// Reference to the associated SkyBox
		SkyBox *m_skybox;

		// Reference to the associated FrameCounter
		FrameCounter *m_framecounter;

		// True if the mouse is to be captured, otherwise false; default is true
		bool m_capture_mouse;
};

/// Counts the FPS of the game and displays them in the title bar
class FrameCounter
{
	public:
		FrameCounter();
		~FrameCounter() {};

		void dispFrame();
		void update_fps();

	private:
		static void onTimerElapse(int ignore);

		// The latest calculated value of the displayed FPS
		int m_fps;

		// Last time a frame was rendered retrieved by glutGet(GLUT_ELAPSED_TIME)
		long m_last_rendertime;
};

#endif
