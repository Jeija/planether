#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <ctime>

#include "environment.hpp"
#include "spaceship.hpp"
#include "gravity.hpp"
#include "config.hpp"

/*
	World Environment
*/

/**
 * \brief Creates a new WorldEnvironment
 *
 * Also spawns the threads for the steps by creating an EnvironmentStepThreadManager
 */
WorldEnvironment::WorldEnvironment()
{
	m_threadman = new EnvironmentStepThreadManager();
}

/**
 * \brief Destructs the WorldEnvironment and all contained objects.
 */
WorldEnvironment::~WorldEnvironment()
{
	delete m_threadman;

	for(std::vector<WorldObject*>::iterator obj = m_objects.begin(); obj!= m_objects.end();)
	{
		if (*obj != nullptr) delete (*obj);
		m_objects.erase(obj);
	}

	std::cout<<"~WorldEnvironment"<<std::endl;
}

/**
 * \brief Calls step on all the Objects.
 * \param dtime The time in seconds that passed since this was called last.
 *
 * Creates a GravityManager for the current WorldEnvironment, calls GenericObject::stepMainThread()
 * all the objects and then tells the EnvironmentStepThreadManager to execute the step() functions
 * in a mulithreaded environment.
 */
void WorldEnvironment::step(float dtime)
{
	m_gravityman = new GravityManager(&m_objects);

	for(auto obj : m_objects)
	{
		obj->stepMainThread(dtime);
		m_threadman->addObject(obj);
	}

	m_threadman->executeStep(dtime);

	/*
		Delete obsolete objects
	*/
	for(std::vector<WorldObject*>::iterator obj = m_objects.begin(); obj!= m_objects.end();)
	{
		if ((*obj)->isObsolete())
		{
			delete (*obj);
			m_objects.erase(obj);
		}
		else ++obj;
	}

	delete m_gravityman;
}

/**
 * \brief Adds an object to the WorldEnvironment
 * \param obj The object to add
 */
void WorldEnvironment::addObject(WorldObject *obj)
{
	m_objects.push_back(obj);
}

/**
 * \brief Retrieve the gravity acceleration for the given position
 *
 * Forward the request to the current GravityManager. Ignores mass of any object
 * that is exactly at pos itself.
 */
SimpleVec3d WorldEnvironment::getGravityAcc(SimpleVec3d pos)
{
	return m_gravityman->getGravityAcc(pos);
}

/*
	Static Environment
*/

/**
 * \brief Calls step() on all objects in the StaticEnvironment
 * \param dtime The time in seconds that passed since this was called last.
 */
void StaticEnvironment::step(float dtime)
{
	for(std::vector<StaticObject*>::iterator obj = m_objects.begin(); obj!= m_objects.end();)
	{
		if ((*obj)->isObsolete())
		{
			delete (*obj);
			m_objects.erase(obj);
		}
		else
		{
			(*obj)->step(dtime);
			++obj;
		}
	}
	processAddObjectQueue();
}

/**
 * \brief Destructs the StaticEnvironment and all contained objects
 */
StaticEnvironment::~StaticEnvironment()
{
	for(std::vector<StaticObject*>::iterator obj = m_objects.begin(); obj!= m_objects.end();)
	{
		if (*obj != nullptr) delete (*obj);
		m_objects.erase(obj);
	}

	std::cout<<"~StaticEnvironment"<<std::endl;
}

/**
 * \brief Calls StaticObject::reshape() on all contained StaticObjects
 * \param width The window width
 * \param height The window height
 */
void StaticEnvironment::reshape(int width, int height)
{
	for (auto obj : m_objects)
		obj->reshape(width, height);
}

/**
 * \brief Adds an object to the StaticEnvironment
 * \param obj The object to add
 *
 * Actually, the object will only be added when all the step()-executions have completed.
 */
void StaticEnvironment::addObject(StaticObject *obj)
{
	m_objects_to_add.push_back(obj);
}

/**
 * \brief Adds all objects that were marked for adding in StaticEnvironment::addObject
 *
 * The objects are only added later on in order not to break the list of objects when
 * addObject() is called from a step() function itself.
 */
void StaticEnvironment::processAddObjectQueue()
{
	for(auto obj : m_objects_to_add)
		m_objects.push_back(obj);

	m_objects_to_add.clear();
}

/*
	EnvironmentStepThreadManager: Multithreaded manager for the step() calls
	in the environments
*/

/**
 * \brief Creates an EnvironmentStepThreadManager and spawns EnvironmentStepThreads
 *
 * The number of threads to spawn is retrieved using std::thread::hardware_concurrency()
 */
EnvironmentStepThreadManager::EnvironmentStepThreadManager ()
{
	// Detect ideal number of parallel threads
	uint16_t thread_num = std::thread::hardware_concurrency();
	if (thread_num == 0) thread_num = 2; // unable to detect, just use 2

	std::cout << "Multithreading: Using " << thread_num << " concurrent threads." << std::endl;

	for (uint16_t i = 0; i < thread_num; ++i)
		m_threads.push_back(new EnvironmentStepThread());
}

/**
 * \brief Destructs the EnvironmentStepThreadManager and all EnvironmentStepThreads
 */
EnvironmentStepThreadManager::~EnvironmentStepThreadManager()
{
	for (auto t : m_threads)
		delete t;
}

/**
 * \brief Tell all EnvironmentStepThreads to execute a step
 * \param dtime The time that passed since this was last called
 */
void EnvironmentStepThreadManager::executeStep(float dtime)
{
	for (auto t : m_threads)
		t->start(dtime, &m_objects);

	for (auto t : m_threads)
		t->waitFinished();

	for (auto obj : m_objects)
		delete obj;

	m_objects.clear();
}

/**
 * \brief Add an object to any EnvironmentStepThread to execute next time executeStep() is called
 * \param object The GenericObject to add
 */
void EnvironmentStepThreadManager::addObject(GenericObject *object)
{
	/*
		Even though it may sound as if adding objects to this takes long,
		it is just a matter of several microseconds
	*/
	m_objects.push_back(new StepThreadObject(object));
}

/*
	EnvironmentStepThread
*/

/**
 * \brief Constructs an EnvironmentStepThread that contains a std::thread and does the communication
 */
EnvironmentStepThread::EnvironmentStepThread() :
m_running(true)
{
	m_state = 0;

	m_thread = new std::thread(&EnvironmentStepThread::executor, this);
}

/**
 * \brief Destructor: stops the contained std::thread
 */
EnvironmentStepThread::~EnvironmentStepThread()
{
	// Continue running the thread, so that it will kill itself
	m_running = false;

	// Set thread in running state and wait for termination
	m_state = 1;

	m_thread->join();
	delete m_thread;
}

/**
 * \brief Starts the execution of the step function for all the objects
 * \param dtime Time since this was last called
 * \param objects A list of objects to call step() on
 */
void EnvironmentStepThread::start(float dtime, std::vector<StepThreadObject *> *objects)
{
	m_objects = objects;
	m_dtime = dtime;

	// Set thread in running state
	m_state = 1;
}

/**
 * \brief Wait until the thread has finished - all Objects received a step() call
 */
void EnvironmentStepThread::waitFinished()
{
	// Wait until m_thread tells the main one it has finished
	while (m_state != 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

/**
 * \brief Function that runs in a seperate std::thread and does the execution of the step() - functions
 */
void EnvironmentStepThread::executor()
{
	while(m_running)
	{
		// Wait until main thread tells this thread that it can continue
		while (m_state != 1) std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!m_running) return;

		// iterate through all objects and execute those steps that are not locked by other
		// threads yet
		for (auto obj : *m_objects)
			obj->tryExecute(m_dtime);

		m_state = 0;
	}
}

/*
	StepThreadObject
*/

/// Creates a new StepThreadObject out of a GenericObject
StepThreadObject::StepThreadObject(GenericObject *obj) :
m_object(obj)
{
};

/// Empty
StepThreadObject::~StepThreadObject()
{
}

/// Executes the step() function if no other thread has done it before (using mutexes)
inline void StepThreadObject::tryExecute(float dtime)
{
	if (m_mutex.try_lock())
		m_object->step(dtime);
}




