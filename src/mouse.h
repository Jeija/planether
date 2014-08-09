#ifndef MOUSE_H
#define MOUSE_H

#ifdef PLANETHER_WINDOWS
	#include <windows.h>
#endif

#include <vector>

void on_mouse_click_default(int button, int state, int x, int y, void *unused);


/// Manages mouse movement and clicks and calls callbacks
class Mouse
{
	public:
		Mouse();

		void onMouseMove(int x, int y);
		void onMouseClick(int button, int state, int x, int y);

		void registerMoveCallback(void (*cb)(int x, int y, void *param), void *param);
		void registerClickCallback(void (*cb)(int btn, int state,  int x, int y, void *param),
			void *param);

#ifdef PLANETHER_WINDOWS // use windows.h for input
		void handleMicrosoftWindows();
#endif

	private:
		std::vector<std::pair<void (*)(int x, int y, void *param), void *>> m_move_callbacks;
		std::vector<std::pair<void (*)(int btn, int state, int x, int y, void *param), void *>>
			m_click_callbacks;

		bool m_ignore;

#ifdef PLANETHER_WINDOWS
		POINT m_centerpos;
#endif
};

#endif
