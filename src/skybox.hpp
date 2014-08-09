#include "gllibs.hpp"
#include "objects.hpp"

#ifndef _SKYBOX_H
#define _SKYBOX_H

/// The star background
class SkyBox : public GenericObject
{
	public:
		SkyBox();
		~SkyBox();

		void render();

	private:
		GLuint m_textures[6];
		GLuint loadTexture(std::string name);
		void renderSide(uint8_t side);

};

#endif
