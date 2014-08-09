#include <string>

#include "objects.h"
#include "util.h"

#ifndef _TELEPORT_H
#define _TELEPORT_H

/// A WorldObject that the player can teleport or navigate to
class TeleportTarget
{
	public:
		TeleportTarget() {};

		virtual std::string	getTeleportName  () { return std::string();    };
		virtual SimpleVec3d	getTeleportPos   () { return SimpleVec3d(); };
		virtual SimpleAngles	getTeleportAngles() { return SimpleAngles();   };

		
		/**
		 * \brief Render a preview of the TeleportTarget in the TeleportWindow
 		 * \param time Set to 0 when first rendered and then increased, in seconds
		 * \param scale Scale factor to be applied (e.g. 0.5 means the objects can have the maximum boundaries -0.5, 0.5; preview field is quadratic)
		 *
		 */
		virtual void renderPreview(float time, float scale) {};
};

// Call this to retrieve the TeleportTarget with name
TeleportTarget *getTeleportTarget(std::string name);

// Call this to retrieve all available TeleportTargets
std::vector<std::string> getAllTeleportTargets();

#endif
