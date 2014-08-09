#include <string>

#include "environment.h"
#include "teleport.h"
#include "gamevars.h"
#include "objects.h"
#include "game.h"

TeleportTarget *getTeleportTarget(std::string name)
{
	// Find TeleportTarget out of WorldEnvironment
	for (auto obj : game->getWorldEnv()->getObjects())
	{
		TeleportTarget *target = dynamic_cast<TeleportTarget *>(obj);
		if (target) // only if object is a TeleportTarget
		{
			if (target->getTeleportName() == name)
				return target;
		}
	}

	return nullptr;
}

std::vector<std::string> getAllTeleportTargets()
{
	std::vector<std::string> targets;
	targets.clear();

	// Find all TeleportTargets out of WorldEnvironment
	for (auto obj : game->getWorldEnv()->getObjects())
	{
		TeleportTarget *target = dynamic_cast<TeleportTarget *>(obj);
		if (target) // only if object is a TeleportTarget
			targets.push_back(target->getTeleportName());
	}

	return targets;
}
