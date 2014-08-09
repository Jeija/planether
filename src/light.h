/*
	Information on Light emitted by a WorldObject;
	Position is WorldObject's m_pos
*/

//#include "gllibs.h"
#include <iostream>
#include <vector>
#include "util.h"

/// Piece of information about a light source
class LightInformation
{
	public:
		LightInformation(GLenum type, bool usefv, const GLfloat *paramfv, GLfloat paramf) :
			m_type		(type),
			m_usefv		(usefv),
			m_paramf	(paramf)
			{
				if (usefv)
				{
					for (uint8_t i = 0; i <= 3; i++)
						m_paramfv[i] = paramfv[i];
				}
			}
		void render(GLenum lightid);


	private:
		GLenum m_type;			// GL_AMBIENT / GL_DIFFUSE / GL_SPECULAR / ...

		bool m_usefv;			// Specifies if we have to use glLightf/v
		GLfloat		m_paramfv[4];	// Effect depending on type
		GLfloat		m_paramf ;	// Effect depending on type
};

/// All LightInformations combined make up the whole light specification
class LightSpec
{
	public:
		LightSpec();
		~LightSpec();

		void render(SimpleVec3d m_pos);
		void disable();
		void enable();

		void addLightInformationfv(GLenum type,  const GLfloat *param);
		void addLightInformationcolor(GLenum type, SimpleColor color);
		void addLightInformationf (GLenum type, GLfloat  param);

		void clearLightInformation()
			{ m_lightinf.clear(); };

		void setLightID(GLenum lightid)
			{ m_lightid = lightid; };

	private:
		std::vector<LightInformation> m_lightinf;
		GLenum m_lightid;
		bool m_enabled;
};
