#include <vector>
#include <string>
#include "gllibs.h"

#ifndef _SHADER_H
#define _SHADER_H

/// A single shader group / shader file
class Shader
{
	public:
		Shader(std::string filename, std::string prepend) :
		m_filename(filename),
		m_prepend(prepend) {};

		void loadShader();
		void loadShaderGroup();

		void use();

		std::string getFileName()
			{ return m_filename; };

		void addParameter3f(std::string name, GLfloat *param);
		void addParameterf(std::string name, GLfloat param);
		void addParameteri(std::string name, GLint param);

	private:
		void throwError(std::string filename, GLuint shader);

		GLuint importShader(std::string filename); // returns shader id
		GLuint m_id;
		std::string m_filename;
		std::string m_prepend;
		std::vector<std::pair<std::string, GLfloat*>> m_parameters_3f;
		std::vector<std::pair<std::string, GLfloat>>  m_parameters_f;
		std::vector<std::pair<std::string, GLint>>  m_parameters_i;
};

/// Manager for all Shaders, compiles and provides them
class ShaderManager
{
	public:
		ShaderManager();

		// request = execute
		void requestShader(std::string filename);

		// get = retrieve Shader class
		Shader *getShader(std::string fileame);
		void resetShader();

	private:
		std::vector<Shader*> m_shaders;
};

#endif
