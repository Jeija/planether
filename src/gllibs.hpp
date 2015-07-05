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
	#ifdef __APPLE__
	#include <OpenGL/glu.h>
	#include <OpenGL/gl.h>

	#include <GLUT/glut.h>
	#include <string.h>
	inline void glutBitmapString(void *font, const unsigned char *string)
	{
		int i = 0;
		while (string[i] != 0)
			glutBitmapCharacter(font, string[i++]);
	}
	inline void glutStrokeString(void *fontID, const unsigned char *string)
	{
		int i = 0;
		while (string[i] != 0)
			glutStrokeCharacter(fontID, string[i++]);
	}
	inline GLfloat glutStrokeHeight ( void *font )
	{
		return 3*glutStrokeWidth(font, 'n');
	}
	inline void glutLeaveMainLoop()
	{
		exit(0);
	}
	#else
	#include <GL/glu.h>
	#include <GL/gl.h>

	#include <GL/freeglut.h>
	#endif

#endif
