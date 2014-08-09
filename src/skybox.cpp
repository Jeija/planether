#include "gllibs.hpp"
#include "skybox.hpp"
#include "gamevars.hpp"

GLfloat cubevertices[] =
{
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f
};

GLubyte cubeindices[6][4] =
{
	{0, 1, 2, 3},
	{4, 0, 3, 5},
	{6, 4, 5, 7},
	{1, 6, 7, 2},
	{0, 4, 6, 1},
	{2, 7, 5, 3}
};

GLfloat texcoords[] =
{
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 0, 1, 0, 1, 1, 0, 1,
	0, 1, 0, 0, 1, 0, 1, 1,
	1, 0, 1, 1, 0, 1, 0, 0
};

SkyBox::SkyBox ()
{
	std::cout<<std::endl;
	std::cout<<"###############"<<std::endl;
	std::cout<<"### SKYBOX  ###"<<std::endl;
	std::cout<<"###############"<<std::endl;

	// Load all textures
	m_textures[0] = loadTexture("skybox_Front.png");
	m_textures[1] = loadTexture("skybox_Left.png");
	m_textures[2] = loadTexture("skybox_Back.png");
	m_textures[3] = loadTexture("skybox_Right.png");
	m_textures[4] = loadTexture("skybox_Top.png");
	m_textures[5] = loadTexture("skybox_Bottom.png");
}

SkyBox::~SkyBox ()
{
	glDeleteTextures(6, m_textures);
	std::cout<<"~SkyBox"<<std::endl;
}

GLuint SkyBox::loadTexture(std::string name)
{
	std::string outname = name;
	while (outname.length() < 20) outname += " ";

	std::cout<<outname<<" [..]"<<std::flush;
	GLuint tex = SOIL_load_OGL_texture (texPath(name).c_str(), SOIL_LOAD_RGBA,
		 SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB);
	std::cout<<"\b\b\bOK]"<<std::endl;
	return tex;
}

void SkyBox::renderSide(uint8_t side)
{
	glBindTexture(GL_TEXTURE_2D, m_textures[side]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Generate some pseudo - Texture - coordinates
	// They are generated from the real texcoords[] (see above) that give the
	// per-quad coordinates for the textures
	GLfloat texcoord_ps[16];
	for (uint8_t i = 0; i < 4; ++i)
	{
		texcoord_ps[cubeindices[side][i]*2  ] = texcoords[side*8 + i*2  ];
		texcoord_ps[cubeindices[side][i]*2+1] = texcoords[side*8 + i*2+1];
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	{
		glVertexPointer  (3, GL_FLOAT, 0, cubevertices);
		glTexCoordPointer(2, GL_FLOAT, 0, texcoord_ps);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, cubeindices[side]);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void SkyBox::render()
{
	#if (USE_SKYBOX_SHADER == 1)
	game->getCamera()->getShaderManager()->requestShader("skybox");
	#endif

	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	{
		glEnable(GL_TEXTURE_2D);
		glDisable (GL_DEPTH_TEST);
		glDisable (GL_BLEND);
		glDisable (GL_LIGHTING);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		for (uint8_t i = 0; i < 6; i++)
			renderSide(i);

	}
	glPopAttrib();
	glPopMatrix();
}
