#include "navigation.hpp"
#include "quatutil.hpp"
#include "teleport.hpp"
#include "config.hpp"
#include "debug.hpp"


Navigator::Navigator() :
m_target_pos(SimpleVec3d()),
m_start_pos(SimpleVec3d()),
m_target(nullptr),
m_active(false),
m_ready(false)
{
	m_indices[0] = 0;
	m_indices[1] = 1;
}

Navigator::~Navigator()
{
	std::cout<<"~Navigator"<<std::endl;
}

void Navigator::setTarget(TeleportTarget *target)
{
	m_target = target;
}

/*
	Build new vertices array
*/
void Navigator::step(SimpleVec3d start_pos, SimpleVec3d translation)
{
	m_start_pos = start_pos;

	if (!m_active) return;
	if (m_target != nullptr)
	{
		m_target_name = m_target->getTeleportName();
		m_target_pos = m_target->getTeleportPos();
	}

	start_pos -= translation;

	m_vertices[0] = start_pos.x;
	m_vertices[1] = start_pos.y;
	m_vertices[2] = start_pos.z;

	m_vertices[3] = (m_target_pos - translation).x;
	m_vertices[4] = (m_target_pos - translation).y;
	m_vertices[5] = (m_target_pos - translation).z;

	m_ready = true;
}

void Navigator::renderPath(glm::quat spaceship_quat)
{
	if (!m_active) return;
	if (!m_ready) return;

	SimpleColor(0.0, 1.0, 0.0).setEmission();

	glLineWidth(1.0f);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	{
		glVertexPointer(3, GL_FLOAT, 0, m_vertices);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_BYTE, m_indices);
	}
	glDisableClientState(GL_VERTEX_ARRAY);

	#define TEXDIST_FROM_START 100. * USC
	#define TEXTDIST_LEN 80. * USC

	SimpleVec3d dirvec = (m_target_pos - m_start_pos).normalize();
	SimpleVec3d textpos = getVectorPerpendicular(dirvec).normalize() * TEXTDIST_LEN;

	glPushMatrix();
	{
		// Position text
		(dirvec * TEXDIST_FROM_START).translate();
		textpos.translate();

		glm::mat4 dirmat = glm::toMat4(spaceship_quat);
		glMultMatrixf(glm::value_ptr(dirmat));

		// Generate text
		float distance = getVectorLength(m_target_pos - m_start_pos);
		std::string diststring = "> ";

		if (distance <= LIGHTMINUTE)
			diststring += std::to_string(distance/USC) + " km";
		else if (distance <= LIGHTYEAR)
			diststring += std::to_string(distance/LIGHTMINUTE) + " lm";
		else
			diststring += std::to_string(distance/LIGHTYEAR) + " ly";

		diststring += " to " + m_target->getTeleportName() + " <";

		glScalef(0.0007, 0.0007, 0.0007);

		// Center text
		float texlen = glutStrokeLength(GLUT_STROKE_MONO_ROMAN,
			(unsigned char *)diststring.c_str());
		glTranslatef(-texlen/2, 0.0, 0.0);

		// Draw text
		glLineWidth(2.0f);
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (unsigned char *)diststring.c_str());
	}
	glPopMatrix();
}

void Navigator::start()
{
	m_active = true;
}

void Navigator::stop()
{
	m_active = false;
}

glm::quat Navigator::getTargetQuat()
{
	SimpleVec3d diffvec = m_target_pos - m_start_pos;
	diffvec.normalize();
	if (fabs(diffvec.x) < 0.01) diffvec.x = 0;
	if (fabs(diffvec.y) < 0.01) diffvec.y = 0;
	if (fabs(diffvec.z) < 0.01) diffvec.z = 0;
	return RotationBetweenVectors(SimpleVec3d(0, 0, -1), diffvec);
}
