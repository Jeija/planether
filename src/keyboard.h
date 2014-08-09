#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>

void process_keyboard_layout_default(bool keystate[255], bool keystate_special[255],
	float dtime, void *param);
void process_keypress_default(unsigned char key, void *param);

/// Manages Keyboard input and calls callbacks
class KeyBoard
{
	public:
		KeyBoard();
		~KeyBoard();

		void processKeys(float dtime);
		void onKeyPress(unsigned char key, int x, int y);
		void onKeyRelease(unsigned char key, int x, int y);
		void onSpecialKeyPress(int key, int x, int y);
		void onSpecialKeyRelease(int key, int x, int y);

		/// Retruns true if the given ASCII key is down
		bool getPressed(char key)
			{ return m_keystate[(uint8_t)key]; }

		/// Retruns true if the given special key is down
		bool getPressedSpecial(char key)
			{ return m_keystate_special[(uint8_t)key]; }

		// The registration functions all return a unique identifier for the callback
		// that can be passed to unRegister to remove the callback

		// Called on every step
		uint32_t registerCallback(void (*callback) (bool keystate[255],
			bool keystate_special[255], float dtime, void *param), 
			void *param=nullptr);

		// Called whenever glutKeyboardFunc / glutKeyboardUpFunc triggers
		uint32_t registerKeyPressCallback(void (*callback)
			(unsigned char key, void *param), void *param=nullptr);
		uint32_t registerKeyReleaseCallback(void (*callback)
				(unsigned char key, void *param), void *param=nullptr);

		// Called whenever glutSpecialFunc / glutSpecialUpFunc triggers
		uint32_t registerSpecialKeyPressCallback(void (*callback)
			(int key, void *param), void *param=nullptr);
		uint32_t registerSpecialKeyReleaseCallback(void (*callback)
			(int key, void *param), void *param=nullptr);

		void unRegister(uint32_t id);

	private:
		bool m_keystate[255];
		bool m_keystate_special[255];

		uint32_t m_callbacks_ind;

		std::map<uint32_t, std::pair<void (*)(bool keystate[255], bool keystate_special[255],
			float dtime, void *), void *>> m_callbacks;

		std::map<uint32_t, std::pair<	void (*)(unsigned char key, void *),
						void *>>
			m_key_press_callbacks;
		std::map<uint32_t, std::pair<	void (*)(unsigned char key, void *),
						void *>>
			m_key_release_callbacks;

		std::map<uint32_t, std::pair<	void (*)(int key, void *),
						void *>>
			m_special_key_press_callbacks;
		std::map<uint32_t, std::pair<	void (*)(int key, void *),
						void *>>
			m_special_key_release_callbacks;
};

#endif
