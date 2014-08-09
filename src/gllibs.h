#ifndef _GLLIBS_H
#define _GLLIBS_H

	// Use local glm  + SOIL libraries to avoid conflicts using Windows
	#define GLM_FORCE_RADIANS
	#include "glm/gtc/matrix_transform.hpp"
	#include "glm/gtx/rotate_vector.hpp"
	#include "glm/gtx/quaternion.hpp"
	#include "glm/gtc/type_ptr.hpp"
	#include "glm/glm.hpp"

	#include "SOIL/SOIL.h"

	#include <GL/glew.h>
	#include <GL/glu.h>
	#include <GL/gl.h>

	#include <GL/freeglut.h>

#endif
