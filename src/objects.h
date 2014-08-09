#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include "util.h"
#include "light.h"

/*
	Object classes
*/
/// Base class for WorldObject and StaticObject, an object that can be removed and requires updates
class GenericObject
{
	public:
		GenericObject() : m_obsolete(false) {};
		virtual ~GenericObject() {};

		/**
		 * \brief To be called regularly in order to update the environment
		 * \param dtime Time that has elapsed between this and the last call, in seconds
		 *
		 * The step functions are executed asynchronously in different threads. Therefore
		 * their order of execution is not specified and may vary each environment step.
		 * The environment stepping function is supposed to trigger the step functions
		 * of all contained objects.
		 */
		virtual void step(float dtime) {}

		/**
		 * \brief Like step(), but executed in main thread
		 *
		 * See GenericObject::step() for more detailed information.
		*/
		virtual void stepMainThread(float dtime) {}

		/**
		 * Returns value of m_obsolete. Should be true if the object is to be deleted
		 * by the managing environment.
		 */
		bool isObsolete () { return m_obsolete; };

	protected:
		bool m_obsolete;
};

// World Object
/*
	Positioned in the world,
	positioning done by the engine

	Can also emit light
*/
/// An objects that is positioned somewhere in the WorldEnvironment
class WorldObject : public GenericObject
{
	public:
		WorldObject() : GenericObject() {};
		virtual ~WorldObject() {};

		/**
		 * \brief To be called by the camera, must render the object on the screen.
		 *
		 * The object will be in a perspective matrix.
		 * In the WorldEnvironment the translation of the object is applied before rendering.
		 */
		virtual void render() {};

		SimpleVec3d getPos()
			{ return m_pos; };

		LightSpec getLightSpec ()
			{ return m_light; };

	protected:
		SimpleVec3d m_pos;
		LightSpec m_light;
};

// Physical Object
/*
	Includes acceleration, speed and move function
*/
/// A WorldObject that can have movement and acceleration
class PhysicalObject : public WorldObject
{
	public:
		PhysicalObject() :
			WorldObject()
			{};

		virtual ~PhysicalObject() {};

	protected:
		void physicalMove(float dtime);
		SimpleVec3d m_velocity;
		SimpleVec3d m_acceleration;
};

/// Object that attracts others with its mass (Planets & Stars)
class MassObject
{
	public:
		MassObject() : m_mass(0) {};

		/// Get the mass of the object.
		double getMass()
			{ return m_mass; };

	protected:
		double m_mass; // in kg
};

// Static Object
/*
	Positioned relative to camera,
	positioning needs to be done by itself (in ::render(x, y))
*/
/// Object that always is in front of the player in the 2D StaticEnvironment
class StaticObject : public GenericObject
{
	public:
		StaticObject() : GenericObject() {};
		virtual ~StaticObject() {};

		/**
		 * \brief To be called by the camera, must render the object on the screen.
		 * \param x The screen width
		 * \param y The screen height
		 *
		 * The object will be render with an orthoganal camera matrix.
		 */
		virtual void render(int x, int y) {};

		/**
		 * \brief To be called when the window is reshaped.
		 * \param x The new screen width
		 * \param y The new screen height
		 */
		virtual void reshape(int x, int y) {};
};

#endif
