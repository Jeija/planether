#ifndef _GAME_H
#define _GAME_H

class Camera;
class Player;
class SpaceShip;
class CrossHair;
class AudioEnvironment;
class WorldEnvironment;
class StaticEnvironment;
class PhysicsInformation;
class BackgroundMusicManager;

/*
	class Game is basically just a simple container for interfaces and especially settings;
	it cooardinates everything
*/

/// The main class - a whole instance of the Planether game
class Game
{
	public:
		Game();
		~Game();

		void init();
		void step();
		static void checklose();

		/// Returns a reference to the SpaceShip
		SpaceShip *getSpaceship()
			{ return m_spaceship; }

		/// Returns a reference to the Player
		Player *getPlayer()
			{ return m_player; }

		/// Returns a reference to the AudioEnvironment
		AudioEnvironment *getAudioEnv()
			{ return m_audio_env; }

		/// Returns a reference to the WorldEnvironment
		WorldEnvironment *getWorldEnv()
			{ return m_world_env; }

		/// Returns a reference to the StaticEnvironment
		StaticEnvironment *getStaticEnv()
			{ return m_static_env; }

		/// Returns a reference to the Camera
		Camera *getCamera()
			{ return m_cam; }

		/// Set whether an overlay captures keyboard input
		void setTportOverlay(bool val)
			{ m_tport_overlay = val; };

		/// Get whether an overlay captures keyboard input
		bool getTportOverlay()
			{ return m_tport_overlay; };

		/// Set the game seed
		void setSeed(int seed)
			{ m_seed = seed; };

		/// Get the game seed
		int getSeed()
			{ return m_seed; };

		/// Set whether the Planets should be displayed as wireframe objects
		void setWireframe(bool val)
			{ m_wireframe = val; };

		/// Get whether the Planets should be displayed as wireframe objects
		int getWireframe()
			{ return m_wireframe; };

		/// Trigger a lose event - the game will be restarted
		void triggerLose()
			{ m_lost = true; }

		/// Get whether the game has been lost
		bool getLost()
			{ return m_lost; }

		/// Retrieve time that has gone by in the simulated universe
		/// since its creation, in seconds
		float getUniverseTime()
			{ return m_time; }

		/// Retrieve time since the game was started, in seconds (real time)
		float getRealTime()
			{ return m_time_real; }

		/// Retrieve the gamespeed (realtime <-> simulation time ratio)
		float getGameSpeed()
			{ return m_speed; }

		void addGameSpeed(float val);

	private:
		Player *m_player;
		SpaceShip *m_spaceship;
		AudioEnvironment *m_audio_env;
		WorldEnvironment *m_world_env;
		StaticEnvironment *m_static_env;
		BackgroundMusicManager *m_background_music;
		CrossHair *m_crosshair;
		PhysicsInformation *m_hud_physics;
		Camera *m_cam;

		bool m_tport_overlay;
		int m_seed;
		bool m_wireframe;
		bool m_lost;

		// Timing
		float m_speed;
		float m_time;
		float m_time_real;
};

#endif
