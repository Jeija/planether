#ifndef HUD_H
#define HUD_H

#include "objects.hpp"
#include <map>

class TeleportTarget;
class Image2d;
class AudioNode;
class Player;

/// Texturable cross in the center of the screen
class CrossHair : public StaticObject
{
	public:
		CrossHair();
		~CrossHair();

		void render(int x, int y);
		void reshape(int width, int height);

		// KeyBoard
		void onSpecialKeyPress(int key);

	private:
		// Wrapper
		static void onSpecialKeyPress_wrapper(int key, void *self);

		bool m_hidden;
		Image2d *m_img;
};

/// Points to planets on the HUD display
class Game;
class PlanetLocator : public StaticObject
{
	public:
		PlanetLocator();

		void render(int x, int y);
		void whileWorldMatrix(int window_w, int window_h);

		// KeyBoard
		void onSpecialKeyPress(int key);

		/// Get whether the HUD planets display is hidden (true) or visible (false)
		bool getHidden()
			{ return m_hidden; }

		std::string getSelected();

	private:
		// List of labels (std::string) with related window positions
		std::map<std::string, glm::vec2> m_labels;

		// Wrapper
		static void onSpecialKeyPress_wrapper(int key, void *self);

		bool m_hidden;
};

/// Text displaying relevant information in the top left of the screen
class PhysicsInformation : public StaticObject
{
	public:
		PhysicsInformation ();
		void render(int x, int y);

		// KeyBoard
		void onSpecialKeyPress(int key);

	private:
		// Wrapper
		static void onSpecialKeyPress_wrapper(int key, void *self);

		bool m_hidden;
};

/// Possible action to execute after the TeleportWindow was closed
class TeleportWindowAction
{
	public:
		TeleportWindowAction(std::string description, void (*action)(TeleportTarget*));
		~TeleportWindowAction() {};
		std::string getDescription()
			{ return m_description; }

		// action to execute when invalid / no target was entered
		void setErrorAction(void (*action)())
			{ m_action_error = action; }
			
		void execute(TeleportTarget *target);
		void executeError();

	private:
		std::string m_description;
		void (*m_action)(TeleportTarget*);
		void (*m_action_error)();
};

/// Window that allows the player to navigate or teleport (open with t by default)
class TeleportWindow : public StaticObject
{
	public:
		TeleportWindow();
		~TeleportWindow();
		void step (float dtime);
		void render(int x, int y);

		// KeyBoard
		void onKeyPress (unsigned char key);
		void onSpecialKeyPress(int key);

	private:
		// Wrappers
		static void onKeyPress_wrapper (unsigned char key, void *self);
		static void onSpecialKeyPress_wrapper(int key, void *param);

		// KeyBoard
		uint32_t m_keyb_callb_id;
		uint32_t m_spec_keyb_callb_id;

		std::string m_destination;
		TeleportTarget *m_target;

		// Time since the preivew was last updated
		float m_preview_time;

		AudioNode *m_audio;

		// Action selector
		std::vector<TeleportWindowAction> m_actions;
		uint16_t m_action_selected;

		/*
			Possible actions after entering the target
		*/
		static void action_teleportTo(TeleportTarget *target); // default
		static void action_teleportSpaceship(TeleportTarget *target); // tp spaceship
		static void action_navigateTo(TeleportTarget *target); // navigator target setting
		static void action_navigateToError(); // reset navigator

		// Available TeleportTargets
		std::vector<std::string> m_available_targets;
};

#endif
