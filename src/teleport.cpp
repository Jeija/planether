#include <string>

#include "environment.hpp"
#include "teleport.hpp"
#include "gamevars.hpp"
#include "objects.hpp"
#include "game.hpp"

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
