/*
	This is quite a primitve class,
	not much more than a container of objects that simplifies
	steps and rendering of different environments.
	Never use Environment, but use WorldEnvironment or
	StaticEnvironment to distinguish between the two types.
*/


#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

#include "util.h"

class WorldObject;
class StaticObject;
class GenericObject;
class GravityManager;
class StepThreadObject;
class EnvironmentStepThread;
class EnvironmentStepThreadManager;

/// Base class for WorldEnvironment and StaticEnvironment, contains Objects
class Environment
{
	public:
		Environment() {};
		virtual ~Environment() {};

		/// Call GenericObject::step() on all contained objects
		virtual void step (float dtime) {};
};

/// Contains and manages all the WorldObjects
class WorldEnvironment  : public Environment
{
	public:
		WorldEnvironment();
		~WorldEnvironment();

		std::vector<WorldObject*> getObjects()
			{ return m_objects; };
		void addObject(WorldObject *obj);
		void step(float dtime);
		SimpleVec3d getGravityAcc(SimpleVec3d pos);

	private:
		std::vector<WorldObject*> m_objects;
		GravityManager *m_gravityman;
		EnvironmentStepThreadManager *m_threadman;
};

/// Contains and manages all the StaticObjects
class StaticEnvironment : public Environment
{
	public:
		StaticEnvironment() {};
		~StaticEnvironment();

		std::vector<StaticObject*> getObjects()
			{ return m_objects; };
		void addObject(StaticObject *obj);
		void step(float dtime);
		void reshape(int width, int height);

	private:
		void processAddObjectQueue();
		std::vector<StaticObject*> m_objects;
		std::vector<StaticObject*> m_objects_to_add;
};

/// Manages the EnvironmentStepThreads
class EnvironmentStepThreadManager
{
	public:
		EnvironmentStepThreadManager();
		~EnvironmentStepThreadManager();

		void addObject(GenericObject *obj);
		void executeStep(float dtime);

	private:
		std::vector<EnvironmentStepThread *> m_threads;
		std::vector<StepThreadObject *> m_objects;
};

/// Processes GenericObjects in a sperate thread (GenericObject::step())
class EnvironmentStepThread
{
	public:
		EnvironmentStepThread();
		~EnvironmentStepThread();
		void start(float dtime, std::vector<StepThreadObject *> *objects);
		void waitFinished();

	private:
		void executor();
		std::atomic<bool> m_running;

		std::thread *m_thread;

		// true state = running, false state = pausing
		std::atomic<bool> m_state;

		float m_dtime;

		std::vector<StepThreadObject *> *m_objects;
};

/// Single GenericObject reference, manages execution of GenericObject::step() within multiple threads
class StepThreadObject
{
	public:
		StepThreadObject(GenericObject *obj);
		~StepThreadObject();

		inline void tryExecute(float dtime);

	private:
		GenericObject *m_object;
		std::mutex m_mutex;
};

#endif
