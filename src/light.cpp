#include "gllibs.h"
#include "light.h"
#include "util.h"

void LightInformation::render (GLenum lightid)
{
	if (m_usefv)
		glLightfv(lightid, m_type, m_paramfv);
	else
		glLightf (lightid, m_type, m_paramf); 
}

LightSpec::LightSpec () :
m_lightid(GL_LIGHT0),
m_enabled(false)
{
	clearLightInformation();
}

LightSpec::~LightSpec ()
{
	clearLightInformation();
}

void LightSpec::render(SimpleVec3d pos)
{
	glEnable(m_lightid);

	GLfloat light_pos[] = {(float)pos.x, (float)pos.y, (float)pos.z, 1.0};
	glLightfv(m_lightid, GL_POSITION, light_pos);

	for (auto li : m_lightinf)
	{
		li.render(m_lightid);
	}
}

void LightSpec::disable()
{
	glDisable(m_lightid);
	m_enabled = false;
}

void LightSpec::enable()
{
	m_enabled = true;
}

void LightSpec::addLightInformationfv(GLenum type, const GLfloat *param)
{
	m_lightinf.push_back(LightInformation(type, true, param, 0));
}

void LightSpec::addLightInformationcolor(GLenum type, SimpleColor color)
{
	const GLfloat param[] = {color.r, color.g, color.b, color.a};
	m_lightinf.push_back(LightInformation(type, true, param, 0));
}

void LightSpec::addLightInformationf(GLenum type, GLfloat param)
{
	m_lightinf.push_back(LightInformation(type, false, NULL, param));
}
