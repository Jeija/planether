#include "gllibs.h"
#include <ctime>

#include "drawutil.h"
#include "gamevars.h"
#include "config.h"
#include "debug.h"
#include "game.h"
#include "util.h"


// Less recursion results in better performance in comparison to quadtrees
#define CHILDREN_NUM 4

#define VERTEX_NO_EXIST 0xffff

/*
	Helpers:
*/

/**
 * \brief Creates a new Image2d with file as picture
 * \param file The file to use as texture
 */
Image2d::Image2d(std::string file)
{
	init(file);
}

/// Deletes OpenGL reference to texture
Image2d::~Image2d()
{
	glDeleteTextures(1, &m_img);
}

/**
 * \brief Creates new image with bounds
 * \param file The file to use as texture
 * \param x The bottom left coordinate, x
 * \param y The bottom left coordinate, y
 * \param w The width of the image
 * \param h The height of the image
 */
Image2d::Image2d(std::string file, float x, float y, float w, float h)
{
	init(file);

	// X - Y
	setBounds(x, y, w, h);
}

/**
 * Loads the texture and prepares the vertices + indices
 */
void Image2d::init(std::string file)
{
	m_img = SOIL_load_OGL_texture (texPath(file).c_str(), SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB);

	// Z - always 1.0
	m_vertices[0][2] = 1.0;
	m_vertices[1][2] = 1.0;
	m_vertices[2][2] = 1.0;
	m_vertices[3][2] = 1.0;

	// Texture X - Texture Y (TexCoords)
	m_texcoords[0][0] = 0.0;
	m_texcoords[0][1] = 0.0;

	m_texcoords[1][0] = 1.0;
	m_texcoords[1][1] = 0.0;

	m_texcoords[2][0] = 1.0;
	m_texcoords[2][1] = 1.0;

	m_texcoords[3][0] = 0.0;
	m_texcoords[3][1] = 1.0;

	// Indices - fill with numbers 0..3
	for (uint8_t i = 0; i < 4; ++i)
		m_indices[i] = i;
}

/**
 * \brief Updates the bounds (=position, size) of the image
 * \param x The bottom left coordinate, x
 * \param y The bottom left coordinate, y
 * \param w The width of the image
 * \param h The height of the image
 */
void Image2d::setBounds(float x, float y, float w, float h)
{
	// Set position (x, y) + size (w, h)
	m_vertices[0][0] = x;
	m_vertices[0][1] = y;

	m_vertices[1][0] = x+w;
	m_vertices[1][1] = y;

	m_vertices[2][0] = x+w;
	m_vertices[2][1] = y+h;

	m_vertices[3][0] = x;
	m_vertices[3][1] = y+h;
}

void Image2d::render()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_img);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	{
		glVertexPointer  (3, GL_FLOAT, 0, m_vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, m_texcoords);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, m_indices);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_TEXTURE_2D);
}

void makeCuboid(SimpleVec3d c1, SimpleVec3d c2, int detailx, int detaily, int detailz)
{
	//printf("makeCuboid\n");
	SimpleVec3d m1, m2, m3, m4, m5, m6, m7, m8;

	// x = rechts / links
	// y = oben / unten
	// z = vorne hinten
	// Eckpunkte berechnen
	m1 = c1;					// links   unten  hinten
	m2 = c2;					// rechts  oben   vorne
	m3 = SimpleVec3d(c1.x, c1.y, c2.z);		// links   unten  vorne
	m4 = SimpleVec3d(c1.x, c2.y, c2.z);		// links   oben   vorne
	m5 = SimpleVec3d(c2.x, c1.y, c1.z);		// rechts  unten  hinten
	m6 = SimpleVec3d(c2.x, c2.y, c1.z);		// rechts  oben   hinten
	m7 = SimpleVec3d(c2.x, c1.y, c2.z);		// rechts  unten  vorne
	m8 = SimpleVec3d(c1.x, c2.y, c1.z);		// links   oben   hinten

	// Zeichnen
	glBegin(GL_TRIANGLES);
	{
		// Unterseite:
		glNormal3f(0, -1, 0);
		cuboidSide(m7, m1, detailx, detaily, detailz);

		// Oberseite
		glNormal3f(0, 1, 0);
		cuboidSide(m2, m8, detailx, detaily, detailz);

		// Rückseite:
		glNormal3f(0, 0, -1);
		cuboidSide(m6, m1, detailx, detaily, detailz);

		// Vorderseite:
		glNormal3f(0, 0, 1);
		cuboidSide(m2, m3, detailx, detaily, detailz);

		// Linksseite:
		glNormal3f(-1, 0, 0);
		cuboidSide(m1, m4, detailx, detaily, detailz);

		// Rechtsseite
		glNormal3f(1, 0, 0);
		cuboidSide(m2, m5, detailx, detaily, detailz);
	}
	glEnd();
}

void makeCylinder(float radius, float zheight, float detail)
{
	glPushMatrix();
	{
		GLUquadric* qobj = gluNewQuadric();
		gluQuadricOrientation(qobj, GLU_OUTSIDE);
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluCylinder(qobj, radius, radius, zheight, detail, detail);
		glPushMatrix();
		{
			glTranslatef(0, 0, zheight);
			gluDisk(qobj, 0, radius, detail, detail);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glRotatef(180, 1, 0, 0);
			gluDisk(qobj, 0, radius, detail, detail);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

inline void drawQuad(SimpleVec3d minp, float x, float y, float z)
{
	// Draws two triangles that make up a quad; one of the parameters x, y, z must be 0

	// One Side:
	glVertex3f(minp.x, minp.y, minp.z);
	if (x!=0) glVertex3f(minp.x+x, minp.y  , minp.z  );
	if (y!=0) glVertex3f(minp.x  , minp.y+y, minp.z  );
	if (z!=0) glVertex3f(minp.x  , minp.y  , minp.z+z);

	if (x!=0) glVertex3f(minp.x+x, minp.y+y, minp.z+z);
	if (z!=0) glVertex3f(minp.x  , minp.y  , minp.z+z);
	if (y!=0) glVertex3f(minp.x  , minp.y+y, minp.z  );
	if (x!=0) glVertex3f(minp.x+x, minp.y  , minp.z  );
	if (x==0) glVertex3f(minp.x+x, minp.y+y, minp.z+z);

	// Other Side:
	if (z!=0) glVertex3f(minp.x  , minp.y  , minp.z+z);
	if (y!=0) glVertex3f(minp.x  , minp.y+y, minp.z  );
	if (x!=0) glVertex3f(minp.x+x, minp.y  , minp.z  );
	glVertex3f(minp.x, minp.y, minp.z);

	if (x==0) glVertex3f(minp.x+x, minp.y+y, minp.z+z);
	if (x!=0) glVertex3f(minp.x+x, minp.y  , minp.z  );
	if (y!=0) glVertex3f(minp.x  , minp.y+y, minp.z  );
	if (z!=0) glVertex3f(minp.x  , minp.y  , minp.z+z);
	if (x!=0) glVertex3f(minp.x+x, minp.y+y, minp.z+z);
}

inline void cuboidSide(SimpleVec3d vec1, SimpleVec3d vec2, int detailx, int detaily, int detailz)
{
	uint8_t sidetype[3] = {1, 1, 1};

	if (vec1.x == vec2.x)
		sidetype[0] = 0;
	if (vec1.y == vec2.y)
		sidetype[1] = 0;
	if (vec1.z == vec2.z)
		sidetype[2] = 0;

	float quadsize_x = (vec1.x - vec2.x) / detailx;
	float quadsize_y = (vec1.y - vec2.y) / detaily;
	float quadsize_z = (vec1.z - vec2.z) / detailz;

	for (int x = 0; x <= detailx*sidetype[0]; x++)
	for (int y = 0; y <= detaily*sidetype[1]; y++)
	for (int z = 0; z <= detailz*sidetype[2]; z++)
	{
		if (z == detailz || y == detaily || x == detailx) continue;
		SimpleVec3d minp;
		minp.x = vec2.x - (1.0*x/detailx) * (vec2.x - vec1.x);
		minp.y = vec2.y - (1.0*y/detaily) * (vec2.y - vec1.y);
		minp.z = vec2.z - (1.0*z/detailz) * (vec2.z - vec1.z);

		drawQuad(minp, sidetype[0]*quadsize_x, sidetype[1]*quadsize_y, sidetype[2]*quadsize_z);
	}
}




/*
	Procedural generation sphere = SphereFraction's
*/

/// Creates a new, yet empty SphereFraction
SphereFraction::SphereFraction() :
m_vertices_id(-1),
m_children_static(false)
{
	// empty constructor for initialization
	m_children.clear();
}

/// Also deletes all children recursively
SphereFraction::~SphereFraction()
{
	for (auto row : m_children)
	for (auto child : row)
		delete child.second;

	m_children.clear();

	if (m_vertices_id != -1)
	{
		free(m_quadvertices[m_vertices_id]);
		//free(m_quadnormals[[m_vertices_id]);
		free(m_quadindices[m_vertices_id]);
	}
}

/**
 * \brief Creates a new SphereFraction
 * \param p The bounds of the new SphereFraction
 *
 * Only to be used to created children SphereFractions.
 */
SphereFraction::SphereFraction(SphericalVector3f *p) :
m_vertices_id(-1),
m_children_static(false)
{
	m_children.clear();

	for (uint8_t i = 0; i < 4; i++)
		m_positions[i] = p[i];

	SphericalVector3f diffvector_spherical(m_positions[0].radius,
		m_positions[1].inclination - m_positions[3].inclination + PI / 2,
		m_positions[1].azimuth - m_positions[3].azimuth);

	SimpleVec3d diffvector = SimpleVec3d(SphericalVector3f(m_positions[0].radius, PI / 2, 0))
		- SimpleVec3d(diffvector_spherical); 

	m_fracsize = getVectorLength(diffvector);
}

/**
 * \brief Create a new SphereFraction with some basic value
 * \param radius Radius of the new SphereFraction
 * \param dyaw Number of columns of children
 * \param dpitch Number of rows of children
 *
 * Only to be created for the outermost SphereFraction
 */
SphereFraction *SphereFraction::makePrototype(float radius, float dyaw, float dpitch)
{
	SphericalVector3f prototype[4];
	prototype[0] = SphericalVector3f(radius, 0,  0);
	prototype[1] = SphericalVector3f(radius, 0,  2*PI);
	prototype[2] = SphericalVector3f(radius, PI, 0);
	prototype[3] = SphericalVector3f(radius, PI, 2*PI);

	SphereFraction *sphere = new SphereFraction(prototype);
	sphere->setChildren(dyaw, dpitch);
	sphere->updateVertices();
	sphere->setChildrenStatic(true);

	return sphere;
}

/// Helper function that find out if a vertex is already included in a list of vertices
inline uint32_t SphereFraction::vertexExistsAlready(GLfloat *vertices, GLuint vertexnum, SimpleVec3d vertex)
{
	if (vertexnum <= CHILDREN_NUM) return VERTEX_NO_EXIST;

	for (uint32_t i = vertexnum - CHILDREN_NUM; i <= vertexnum; i++)
	{
		// if the vectors are very close to each other (less than 100m IRL) they
		// are likely the same, so do not add another vector; slight differences
		// in vectors occur during the calculations, athough they are displayed the same
		if	(   almostEqual(vertices[i*3  ], vertex.x, USC * 0.1)
			 && almostEqual(vertices[i*3+1], vertex.y, USC * 0.1)
			 && almostEqual(vertices[i*3+2], vertex.z, USC * 0.1))
		{
			return i;
		}
	}

	return VERTEX_NO_EXIST;
}

/// Insert a vertex into a list of vertices that make up the SphereFraction
inline void SphereFraction::arrayInsertVector(int i, int *vid, GLfloat *m_quadvertices,
		GLuint *m_quadindices, SimpleVec3d vertex)
{
	uint32_t vertexnum = vertexExistsAlready(m_quadvertices, *vid, vertex);

	if (vertexnum == VERTEX_NO_EXIST)
	{
		m_quadindices [i       ] = *vid;
		m_quadvertices[*vid*3  ] = vertex.x;
		m_quadvertices[*vid*3+1] = vertex.y;
		m_quadvertices[*vid*3+2] = vertex.z;
		(*vid)++;
	}
	else
	{
		m_quadindices[i] = vertexnum;
	}
}

/**
 * \brief Updates the vertices and indices arrays
 *
 * Only to be called for the outermost SphereFraction
 */
void SphereFraction::updateVertices()
{
	std::vector<std::map<int, SimpleVec3d *>> unprocessed_vert = getVertices();

	size_t size = 0;
	for (auto &row : unprocessed_vert)
		size += row.size() * 4; // *4 for 4 vertices per QUAD

	int8_t vertices_id;
	if (m_vertices_id == 1)	vertices_id = 0;
	else			vertices_id = 1;

	/*
		Generate vertex, normal, index buffer for OpenGL (*3 = 3 Dimensions)
		divide by 2 as every second index just points to a already saved
		vertex
	*/
	m_quadvertices[vertices_id] = (GLfloat *)calloc(size / 2, 3 * sizeof(GLfloat));
	//m_quadnormals [vertices_id] = (GLfloat *)calloc(size / 2, 3 * sizeof(GLfloat));
	m_quadindices [vertices_id] = (GLuint  *)calloc(size, 3 * sizeof(GLuint ));

	int i   = 0; // index id
	int vid = 0; // vertex id

	for (auto &row : unprocessed_vert)
	{
		for (auto &strip : row)
		{
			arrayInsertVector(i++, &vid, m_quadvertices[vertices_id],
				m_quadindices[vertices_id], strip.second[0]);
			arrayInsertVector(i++, &vid, m_quadvertices[vertices_id],
				m_quadindices[vertices_id], strip.second[2]);
			arrayInsertVector(i++, &vid, m_quadvertices[vertices_id],
				m_quadindices[vertices_id], strip.second[3]);
			arrayInsertVector(i++, &vid, m_quadvertices[vertices_id],
				m_quadindices[vertices_id], strip.second[1]);

			delete [] strip.second;
		}
	}

	unprocessed_vert.clear();
	m_vertexnum[vertices_id] = i;

	int8_t old_id = m_vertices_id;

	m_mutex.lock();
	m_vertices_id = vertices_id;

	if (old_id != -1)
	{
		free(m_quadvertices[old_id]);
		//free(m_quadnormals [old_id]);
		free(m_quadindices[old_id]);
	}
	m_mutex.unlock();
}

/**
 * Find out if the current number of children at a given position should be changed
 * (player has moved relative to it)
 */
bool SphereFraction::autoNumUpdateReq(SimpleVec3d campos_relative, float intensity,
		SimpleVec3d position)
{
	float camdist = getVectorLength(campos_relative - position);
	if (camdist == 0) return false; /* prevent infinite children */
	return camdist < m_fracsize * intensity;
}

/**
 * \brief Automatically update the number of children depending on the player's position
 * \param campos_rel The relative camera position to the SphereFraction
 * \param intensity The intensity of creating small children SphereFractions; the higher, the more
 *
 * This function is supposed to be called only for the outermost SphereFraction.
 */
bool SphereFraction::autoChildrenNum(SimpleVec3d campos_rel, float intensity)
{
	bool upd_vert_req = false;

	/*
		update_this_req = true if this vertex should have multiple children
	*/
	bool update_req = autoNumUpdateReq(campos_rel, intensity, SimpleVec3d(m_positions[0]));

	if (update_req && m_children.size() != CHILDREN_NUM + 1)
	{
		upd_vert_req = true;
		setChildren(CHILDREN_NUM, CHILDREN_NUM);
	}
	else if (!update_req && m_children.size() == CHILDREN_NUM + 1)
	{
		upd_vert_req = true;
		setChildren(0, 0);
	}

	if (update_req)
	{
		for (auto &row : m_children)
		for (auto &child : row)
		{
			upd_vert_req |= child.second->autoChildrenNum(campos_rel, intensity);
		}
	}

	return upd_vert_req;
}

/**
 * Render the SphereFraction with all the children. Uses glDrawElements for speed.
 * This is supposed to be called by the object that owns the SphereFraction, SphereFraction
 * is not a WorldObject and therefore not rendered automatically.
 */
void SphereFraction::render()
{
	if (m_vertices_id == -1) return; // no vertices have been built so far

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	/*
		Also use vertices as normals, because they point in the same direction as
		the surface (regarding the SphereFraction planet as a perfect sphere
	*/
	m_mutex.lock();
	glVertexPointer(3, GL_FLOAT, 0,	m_quadvertices[m_vertices_id]);
	glNormalPointer(GL_FLOAT, 0,	m_quadvertices[m_vertices_id]);

	if (!game->getWireframe())
		glDrawElements(GL_QUADS, m_vertexnum[m_vertices_id], GL_UNSIGNED_INT,
			m_quadindices[m_vertices_id]);
	else
		glDrawElements(GL_LINE_STRIP, m_vertexnum[m_vertices_id], GL_UNSIGNED_INT,
			m_quadindices[m_vertices_id]);

	m_mutex.unlock();

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/*
	This function is only to be called for the outermost SphereFraction that contains
	all the others
*/
/// Get vertices of the SphereFraction
std::vector<std::map<int, SimpleVec3d *>> SphereFraction::getVertices()
{
	std::vector<std::map<int, SimpleVec3d *>> vertices;

	if (containsChildren())
	{
		vertices = getVerticesContainer();
	}
	else
	{
		std::map<int, SimpleVec3d *> thisrow;
		thisrow[0] = getVerticesChild();

		vertices.push_back(thisrow);
	}

	return vertices;
}

/**
 *	retrieves vertices from a SphereFraction that has children;
 *	the std::vector contains the rows, the std::map contains the quad vertices from 0 - n
 *	in a pair
 */
std::vector<std::map<int, SimpleVec3d*>> SphereFraction::getVerticesContainer()
{
	std::vector<std::map<int, SimpleVec3d*>> vertices;

	for (auto &row : m_children)
	{
		std::map<int, SimpleVec3d *> verticesrow;
		int vri = 0;

		for (auto &child : row)
		{
			if (child.second->containsChildren())
			{
				std::vector<std::map<int, SimpleVec3d *>>
					childvertices = child.second->getVerticesContainer();
				vertices.insert(vertices.end(), childvertices.begin(),
					childvertices.end());
				childvertices.clear();
			}
			else
			{
				verticesrow[vri++] = child.second->getVerticesChild();
			}
		}

		vertices.push_back(verticesrow);
		verticesrow.clear();
	}

	return vertices;
}


/**
 * \brief Retrieves vertices from a SphereFraction that has no children
 * \return SimpleVec3d [4] of the four corners of the SphereFraction
 */

SimpleVec3d *SphereFraction::getVerticesChild()
{
	SimpleVec3d *vertices = new SimpleVec3d[4];

	vertices[0]  = m_positions[0];
	vertices[1]  = m_positions[1];
	vertices[2]  = m_positions[2];
	vertices[3]  = m_positions[3];

	return vertices;
}

/**
 * \brief Set the amount of children of a SphereFraction
 * \param yawres Number of columns
 * \param pitchres Number of rows
 */
void SphereFraction::setChildren(int yawres, int pitchres)
{
	// do not change children if m_children_static is activated
	if (m_children_static) return;

	for (auto &row : m_children)
	for (auto child : row)
		delete child.second;

	m_children.clear();

	if (yawres == 0 || pitchres == 0) return;

	float pitchperc1 = 0;
	float pitchperc2 = 0;

	for (int pitchn = 0; pitchn <= pitchres; pitchn++)
	{
		std::map<int, SphereFraction *> row;
		row.clear();

		pitchperc1 = pitchperc2;
		pitchperc2 = (pitchn * 1.) / pitchres;

		float yawperc = 0;

		SphericalVector3f p[4]; // contains children's positions
		p[0] = getChildPosition(m_positions, 0, pitchperc1);
		p[2] = getChildPosition(m_positions, 0, pitchperc2);

		int yawn = 0;
		for (; yawn <= yawres + 1; yawn++)
		{
			yawperc = (yawn * 1.) / yawres;

			p[1] = getChildPosition(m_positions, yawperc, pitchperc1);
			p[3] = getChildPosition(m_positions, yawperc, pitchperc2);

			row[yawn] = new SphereFraction(p);

			p[0] = p[1];
			p[2] = p[3];
		}

		m_children.push_back(row);
	}
}

/**
 * Interpolate between two azimuth / inclination values to get the SphericalVector3f position for a
 * child SphereFraction.
 */
inline SphericalVector3f SphereFraction::getChildPosition(SphericalVector3f *p, float yawperc, float pitchperc)
{
	float azi = linInterpolate(p[0].azimuth, p[3].azimuth, yawperc);
	float inc = linInterpolate(p[0].inclination, p[3].inclination, pitchperc);

	return SphericalVector3f(p[0].radius, inc, azi);
}

/**
 * \brief Linear interpolation between two float values
 * \param val1 First value
 * \param val2 Second Value
 * \param perc Between 0 (like val1) and 1 (val2)
 */
inline float SphereFraction::linInterpolate(float val1, float val2, float perc)
{
	return val1 + (val2 - val1) * perc;
}
