#include <map>
#include <vector>
#include <mutex>

#include "util.h"

#ifndef _DRAWUTIL_H
#define _DRAWUTIL_H

/// A 2d image to be drawn by a StaticObject
class Image2d
{
	public:
		Image2d(std::string file);
		Image2d(std::string file, float x, float y, float w, float h);
		~Image2d();

		void render();
		void setBounds(float x, float y, float w, float h);

	private:
		void init(std::string file);

		GLuint m_img;
		GLfloat m_vertices[4][3];
		GLfloat m_texcoords[4][2];
		GLubyte m_indices[4];
};

// Draw 3d cuboid
// all coordintates of c1 must be smaller than those of c2
void makeCuboid(SimpleVec3d c1, SimpleVec3d c2, int detailx, int detaily, int detailz);

// Draw 3d cylinder with caps; height ist in the z direction
void makeCylinder(float radius, float zheight, float detail);

// Helper functions for makeCuboid
inline void drawQuad(SimpleVec3d minp, float x, float y, float z);
inline void cuboidSide(SimpleVec3d vec1, SimpleVec3d vec2, int detailx, int detaily, int detailz);

// make a cuboid with only 2 triangles per side; less cpu intensive (even less than makeCuboid with detail=1, as that draws 4 triangles per side)
void makeCuboidSimple(SimpleVec3d c1, SimpleVec3d c2);

// Procedural generation sphere
void makeProceduralSphere(float radius, float dyaw, float dpitch);

/*
	concerning: SphereFraction::setChildren, bool *forcepos[4]
	When calling setChildren, do not dipslay all vertices on the given side (side [0] - [3])
	to interpolate different numbers of children in adjacent containers
*/

/// A quad on a sphere surface, may also contain subdivisions of itself for procedural generation
class SphereFraction
{
	public:
		SphereFraction();
		~SphereFraction();

		SphereFraction(SphericalVector3f *p); // p[4], array of the 4 positions

		static SphereFraction *makePrototype(float radius, float dyaw, float dpitch);
		void render();
		void setChildren(int yawres, int pitchres);
		bool containsChildren()
			{ return m_children.size() > 0; };
		void updateVertices();
		std::vector<std::map<int, SphereFraction *>> *getChildren()
			{ return &m_children; };

		/*
			campos_relative is the relative position of the camera to the origin
			of the sphere's coordinate system (including translations)
			autoChildrenNum automatically adds and removes children
			based on how far away the camera is from the fraction

			returns true if a updateVertices() is required afterwards
		*/
		bool autoChildrenNum(SimpleVec3d campos_relative, float intensity);
		void setChildrenStatic(bool value)
			{ m_children_static = value; };

	private:
		inline static float linInterpolate(float val1, float val2, float perc);
		inline static SphericalVector3f getChildPosition
				(SphericalVector3f *p, float yawperc, float pitchperc);

		/*
			Determine if an update of children numbers would be required if
			the given vertex is at position; helper function
		*/
		bool autoNumUpdateReq(SimpleVec3d campos_relative, float intensity,
			SimpleVec3d position);

		/*
			Checks if vertex is already contained in vertices and returns index if
			it is already there, otherwise returns VERTEX_NO_EXIST
		*/
		inline static uint32_t vertexExistsAlready(GLfloat *vertices, GLuint vertexnum, SimpleVec3d vertex);

		/*
			Adds a vertex to an array, helper function for updateVertices()
		*/
		inline static void arrayInsertVector(int i, int *vid,
			GLfloat *m_quadvertices, GLuint *m_quadindices, SimpleVec3d vertex);

		std::vector<std::map<int, SimpleVec3d *>> getVertices();
		std::vector<std::map<int, SimpleVec3d *>> getVerticesContainer();

		SimpleVec3d* getVerticesChild(); // returns 4 vertices

		SphericalVector3f m_positions[4];
		std::vector<std::map<int, SphereFraction *>> m_children;

		/*
			m_quadvertices / m_quadnormals / m_quadindices
			contains the elements for a QUAD_STRIP, but is only used on the outermost
			SphereFraction, that is not a child of any other one
			Contains 2 arrays of the vertices / normals / indices / uint of vertexnum
			so that one can always be updated while the other is in use. Which one is to
			be used is saved in m_vertices_id; if m_vertices_id == -1, no vertices have
			been built so far (--> no rendering)
		*/
		GLfloat *m_quadvertices[2];
		//GLfloat *m_quadnormals[2];
		GLuint  *m_quadindices[2];
		GLuint   m_vertexnum[2];
		float m_fracsize; // diagonal length through quad IF IT WAS AT THE EQUATOR
				  // (so that all nth children have the same fracsize)

		int8_t m_vertices_id;

		/*
			Mutex is only locked, while rendering (transferring the vertices / normals /
			indices arrays)
		*/
		std::mutex m_mutex;

		/* Don't change number of children if m_children_static is true
			(used for outermost Fraction) */
		bool m_children_static;
};

// Coordinates for a dodecahedron, used for drawing the planetary rings
// This is based on the implementation of glutSolidDodecahedron
const float dodecahedron_normals[12][3] =
{
	{0.0, 		 0.52573,	 0.85065},
	{0.0, 		 0.52573,	-0.85065},
	{0.0, 		-0.52573,	 0.85065},
	{0.0, 		-0.52573,	-0.85065},
	{0.85065,	 0.0,		 0.52573},
	{-0.85065,	 0.0,		 0.52573},
	{0.85065,	 0.0,		-0.52573},
	{-0.85065,	 0.0,		-0.52573},
	{0.52573,	 0.85065,	 0.0},
	{0.52573, 	-0.85065,	 0.0},
	{-0.52573, 	 0.85065,	 0.0},
	{-0.52573,	-0.85065,	 0.0}
};

const float dodecahedron_vertices[12*5][3] =
{
	// Face 1
	{0.0, 		 1.61803,	 0.61803},
	{-1.0, 		 1.0,		 1.0},
	{-0.61803,	 0.0,		 1.61803},
	{0.61803,	 0.0,		 1.61803},
	{1.0, 		 1.0,		 1.0},
	// Face 2
	{0.0, 		 1.61803,	-0.61803},
	{1.0, 		 1.0,		-1.0},
	{ 0.61803,	 0.0,		-1.61803},
	{-0.61803,	 0.0,		-1.61803},
	{-1.0, 		 1.0,		-1.0},
	// Face 3
	{0.0, 		-1.61803,	 0.61803},
	{1.0, 		-1.0,		 1.0},
	{0.61803,	 0.0,		 1.61803},
	{-0.61803,	 0.0,		 1.61803},
	{-1.0, 		-1.0,		 1.0},
	// Face 4
	{0.0, 		-1.61803,	-0.61803},
	{-1.0,		-1.0,		-1.0},
	{-0.61803,	 0.0,		-1.61803},
	{0.61803,	 0.0,		-1.61803},
	{1.0, 	-	 1.0,		-1.0},
	// Face 5
	{0.61803,	 0.0,		 1.61803},
	{1.0, 		-1.0,		 1.0},
	{1.61803,	-0.61803,	 0.0},
	{1.61803,	 0.61803,	 0.0},
	{1.0, 		 1.0,		 1.0},
	// Face 6
	{-0.61803, 	 0.0,		 1.61803},
	{-1.0, 		 1.0,		 1.0},
	{-1.61803, 	 0.61803,	 0.0},
	{-1.61803,	-0.61803,	 0.0},
	{-1.0,		-1.0,		 1.0},
	// Face 7
	{0.61803,	 0.0,		-1.61803},
	{1.0, 		 1.0,		-1.0},
	{1.61803,	 0.61803,	 0.0},
	{1.61803,	-0.61803,	 0.0},
	{1.0, 		-1.0,		-1.0},
	// Face 8
	{-0.61803,	 0.0,		-1.61803},
	{-1.0, 		-1.0,		-1.0},
	{-1.61803,	-0.61803,	 0.0},
	{-1.61803, 	 0.61803,	 0.0},
	{-1.0, 		 1.0,		-1.0},
	// Face 9
	{1.61803,	 0.61803,	 0.0},
	{1.0, 		 1.0,		-1.0},
	{0.0, 		 1.61803,	-0.61803},
	{0.0, 		 1.61803,	 0.61803},
	{1.0,		 1.0,		 1.0},
	// Face 10
	{1.61803,	-0.61803,	 0.0},
	{1.0, 		-1.0,		 1.0},
	{0.0, 		-1.61803,	 0.61803},
	{0.0, 		-1.61803,	-0.61803},
	{1.0,		-1.0,		-1.0},
	// Face 11
	{-1.61803,	 0.61803,	 0.0},
	{-1.0,		 1.0,		 1.0},
	{0.0,		 1.61803,	 0.61803},
	{0.0,		 1.61803,	-0.61803},
	{-1.0,		 1.0,		-1.0},
	// Face 12
	{-1.61803,	-0.61803,	 0.0},
	{-1.0, 		-1.0,		-1.0},
	{0.0, 		-1.61803,	-0.61803},
	{0.0,		-1.61803,	 0.61803},
	{-1.0, 		-1.0,		 1.0}
};

const uint8_t dodecahedron_indices[5] = {0, 1, 2, 3, 4};

#endif
