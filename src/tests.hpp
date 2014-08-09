#ifndef TESTS_H
#define TESTS_H

#include "objects.hpp"
#include "player.hpp"
#include "util.hpp"

void mouse_trigger_shot(int button, int state, int x, int y, void *unused);

/// Testing only!
class Bullet : public PhysicalObject
{
	public:
		Bullet (Player *player);
		~Bullet();

		void render ();
		void step (float dtime);

	protected:
		float m_time;
};

#endif
