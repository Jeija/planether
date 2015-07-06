#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "config.hpp"

#ifndef _ALLIBS_H
#define _ALLIBS_H

#ifdef USE_ALUT
	#include <AL/alut.h>
#endif

#endif
