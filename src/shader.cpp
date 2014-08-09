#include <iostream>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <string>

#include "gllibs.h"

#include "gamevars.h"
#include "shader.h"
#include "config.h"
#include "util.h"
#include "game.h"

/*
	Shader Class
*/

/**
 * \brief Output formatted debug information on failure and exit game
 * \param filename The shader's filename the bug was discovered in
 * \param shader The OpenGL handle to the shader
 *
 * Outputs the shader's compiler's InfoLog text. But first has to change
 * the line numbers of the log as the builtin shader is prepended to all
 * shaders before being compiled - the would be wrong otherwise.
 */
void Shader::throwError(std::string filename, GLuint shader)
{
	/*
	The original error messages created by OpenGL are wrong as they do not take
	into account that the builtin shader is prepended. Therefore, line numbers
	must be corrected internally
	*/

	// Get line number of prepended shader string to subtract it later
	uint16_t prepend_linenum = 1;
	std::ifstream prependstream(m_prepend);

	for (uint16_t i = 0; i<m_prepend.length(); ++i)
		if (m_prepend[i] == '\n') ++prepend_linenum;

	char errorlog_c[4096];
	glGetShaderInfoLog(shader, 4096, nullptr, errorlog_c);
	std::string errorlog(errorlog_c);

	std::string errorlog_correct;
	std::istringstream errorlog_stream(errorlog);

	std::string logline;
	while (std::getline(errorlog_stream, logline, '\n'))
	{
		// Extract line number in error and modify it to fit the builtin shader
		uint8_t linenum_end = logline.find(")");
		uint16_t linenum = std::stoi(logline.substr(2, linenum_end - 2));

		if (int32_t(linenum - prepend_linenum) < 0)
			errorlog_correct += std::string(BUILTIN_SHADER_PATH) + ": " + logline + "\n";
		else
		{
			logline.erase(2, linenum_end - 2);
			logline.insert(2, std::to_string(linenum - prepend_linenum));
			errorlog_correct += filename + ": " + logline + "\n";
		}
	}

	// Output text
	std::cout<<"--------------------------------------"<<std::endl;
	std::cout<<errorlog_correct;
	std::cout<<"--------------------------------------"<<std::endl;
	std::exit(EXIT_FAILURE);
}

/**
 * \brief Creates and compiler a single shader from the file at filename
 * \param filename The GLSL file to compile
 *
 * Detects whether vertex or fragment shader by the file name extension:
 * .glslf = Fragment shader, .glslv = Vertex shader
 */
GLuint Shader::importShader(std::string filename)
{
	GLuint shaderid;

	std::ifstream src;
	std::stringstream srcbuf;
	std::string srcstr;

	src.open(getBasedir() + SHADER_DIR + filename);
	srcbuf << src.rdbuf();
	srcstr = srcbuf.str();
	srcstr = m_prepend + srcstr; // prepend builtin shader

	std::string fn_ext;
	fn_ext = filename.substr(filename.length()-5, 5);

	GLenum shader_type;
	if (fn_ext == "glslf")
		shader_type = GL_FRAGMENT_SHADER;
	else if (fn_ext == "glslv")
		shader_type = GL_VERTEX_SHADER;
	else
	{
		std::cout<<"Shader with invalid filename extension, exiting: "<<filename<<std::endl;
		std::exit(EXIT_FAILURE);
	}

	shaderid = glCreateShader(shader_type);
	const char * srcchar = srcstr.c_str();
	glShaderSource(shaderid, 1, &srcchar, NULL);

	glCompileShader(shaderid);
	GLint compile_status;
	glGetShaderiv(shaderid, GL_COMPILE_STATUS, &compile_status);

	if (compile_status == GL_FALSE) // Shader compilation failed, get Debug information
		throwError(filename, shaderid);

	return shaderid;
}

/**
 * \brief Compiles and links the shader.
 *
 * Uses Shader::importShader as helper function.
 */
void Shader::loadShader()
{
	GLuint shaderid = importShader(m_filename);

	m_id = glCreateProgram();
	glAttachShader(m_id, shaderid);

	glLinkProgram(m_id);
}

/**
 * \brief Imports a shader group
 *
 * A shader group are a vertex and a fragments shader in a single folder that are linked together.
 */
void Shader::loadShaderGroup()
{
	DIR *dir;
	struct dirent *file;

	dir = opendir((getBasedir() + SHADER_DIR + m_filename).c_str());
	assert(dir);

	std::vector<GLuint> shaderids;

	while (( file = readdir(dir) ))
	{
		char firstchar = file->d_name[0];
		char lastchar  = file->d_name[strlen(file->d_name)-1];
		if (firstchar == '.' || lastchar == '~') continue;

		std::string shaderfile = m_filename + DIR_DELIM + file->d_name;
		std::cout<<"|- Group Shader: "<<file->d_name<<std::endl;
		shaderids.push_back(importShader(shaderfile));
	}

	m_id = glCreateProgram();
	for (auto shaderid : shaderids)
	{
		glAttachShader(m_id, shaderid);
	}

	glLinkProgram(m_id);
}

/**
 * \brief Uses the requested shader
 *
 * Calls glUseProgram() for the shader object
 */
void Shader::use()
{
	glUseProgram(m_id);

	GLint time_loc = glGetUniformLocation(m_id, "time");
	if (time_loc != -1)
		glUniform1f(time_loc, game->getUniverseTime());

	GLint usc_loc = glGetUniformLocation(m_id, "usc");
	if (usc_loc != -1)
		glUniform1f(usc_loc, USC);

	GLint seed_loc = glGetUniformLocation(m_id, "seed");
	if (seed_loc != -1)
		glUniform1i(seed_loc, game->getSeed());

	for (auto parami : m_parameters_i)
	{
		GLint piloc = glGetUniformLocation(m_id, parami.first.c_str());
		glUniform1i(piloc, parami.second);
	}	

	for (auto param3f : m_parameters_3f)
	{
		GLint p3floc = glGetUniformLocation(m_id, param3f.first.c_str());
		glUniform3f(p3floc, param3f.second[0], param3f.second[1], param3f.second[2]);
	}

	for (auto paramf : m_parameters_f)
	{
		GLint pfloc = glGetUniformLocation(m_id, paramf.first.c_str());
		glUniform1f(pfloc, paramf.second);
	}

	// clear parameters, need to be resent after every use() call
	m_parameters_3f.clear();
	m_parameters_f.clear();
	m_parameters_i.clear();
}

/**
 * \brief Adds a vec3 parameter for the shader. Will be transferred next time the shader is used.
 * \param name The variable name (location) to put the value in the shader.
 * \param param The value to write to it.
 */
void Shader::addParameter3f(std::string name, GLfloat *param)
{
	m_parameters_3f.push_back(std::pair<std::string, GLfloat*>(name, param));
}

/**
 * \brief Adds a float parameter for the shader. Will be transferred next time the shader is used.
 * \param name The variable name (location) to put the value in the shader.
 * \param param The value to write to it.
 */
void Shader::addParameterf(std::string name, GLfloat param)
{
	m_parameters_f.push_back(std::pair<std::string, GLfloat>(name, param));
}

/**
 * \brief Adds an integer parameter for the shader. Will be transferred next time the shader is used.
 * \param name The variable name (location) to put the value in the shader.
 * \param param The value to write to it.
 */
void Shader::addParameteri(std::string name, GLint param)
{
	m_parameters_i.push_back(std::pair<std::string, GLint>(name, param));
}


/*
	ShaderManager Class
*/

/**
 * \brief Loads and compiles all shaders
 *
 * All shaders are compiled by this function when the game is started. This automatically handles
 * shaders groups and prepending the builtin shader. Shaders have to be in <basedir>/SHADER_DIR,
 * builtin shader at <basedir>/BUILTIN_SHADER_PATH.
 */
ShaderManager::ShaderManager ()
{
	std::cout << std::endl;
	std::cout << "###############" << std::endl;
	std::cout << "### SHADERS ###" << std::endl;
	std::cout << "###############" << std::endl;

	// Load builtin shader, it will be prepended to all the loaded shaders
	std::cout << "Loading builtin shader..." << std::endl;
	std::ifstream builtin_src;
	std::stringstream builtin_buf;
	std::string builtin_str;

	builtin_src.open(getBasedir() + BUILTIN_SHADER_PATH);
	if (!builtin_src)
	{
		std::cout	<< "[ERROR] Could not find builtin shader at "
				<< BUILTIN_SHADER_PATH << std::endl;
		std::exit(EXIT_FAILURE);
	}
	builtin_buf << builtin_src.rdbuf();
	builtin_str = builtin_buf.str();

	DIR *dir;
	struct dirent *file;

	dir = opendir((getBasedir() + SHADER_DIR).c_str());
	assert(dir);

	while (( file = readdir(dir) ))
	{
		char firstchar = file->d_name[0];
		char lastchar  = file->d_name[strlen(file->d_name)-1];

		if (firstchar != '.' && lastchar != '~') // not .. ; not . ; not .hidden ; not hidden~
		{
			std::string filepath = getBasedir() + SHADER_DIR + std::string(file->d_name);
			Shader *s = new Shader(std::string(file->d_name), builtin_str);
			if(opendir(filepath.c_str()))	// must be shader group,
							// glslv + glslf shader in a folder
			{
				std::cout <<	"Found Shader Group: " << file->d_name << std::endl;
				s->loadShaderGroup();
			}

			m_shaders.push_back(s);
		}
	}

	closedir (dir);
}

/**
 * \brief Calls glUseProgram() on the requested shader
 * \param The filename / directory name of the shader to load
 *
 * Looks for the requested shader and loads it if available. Throws error and exits if not found.
 */
void ShaderManager::requestShader(std::string filename)
{
	for (auto shader : m_shaders)
	{
		if (shader->getFileName() == filename)
		{
			shader->use();
			return; // Shader found
		}
	}

	std::cout<<"Error: Shader '"<<filename<<"' not found!"<<std::endl;
	std::exit(EXIT_FAILURE);
}

/**
 * \brief Returns the instance of the requested shader
 * \parameter filename The filename / directory name of the shader to retrieve
 */
Shader *ShaderManager::getShader(std::string filename)
{
	for (auto shader : m_shaders)
	{
		if (shader->getFileName() == filename)
		{
			return shader;
		}
	}

	std::cout<<"Error: Shader '"<<filename<<"' not found in getShader()!"<<std::endl;
	std::exit(EXIT_FAILURE);
}

/**
 * \brief Makes the game only use the default shader again after calling
 */
void ShaderManager::resetShader()
{
	glUseProgram(0);
	requestShader("default");
}
