#define JSON_NO_AUTOLINK
#define JSON_IN_CPPTL
#include <fstream>
#include <sstream>

#include "rapidjson/document.h"
#include "config.hpp"
#include "util.hpp"

/**
 * \brief Initializes the ConfigurationManager that allows to get config options
 *
 * Reads the configuration file (config/config.json by default) and exits with an
 * error if parsing fails.
 */
ConfigurationManager::ConfigurationManager()
{
	std::cout<<"Loading "<<CONFIG_PATH<<"..."<<std::endl;

	// Open config file, read into json_str
	std::ifstream file;
	std::stringstream filebuf;
	std::string json_code;

	file.open(getBasedir() + CONFIG_PATH);
	assert(file != nullptr);

	filebuf << file.rdbuf();
	json_code = filebuf.str();

	// Try to parse configuration, exit with error if it fails
	m_json.Parse<0>(json_code.c_str());

	if (!m_json.IsObject())
	{
		std::cout << "##########################" << std::endl;
		std::cout << "## Error parsing config ##" << std::endl;
		std::cout << "##########################" << std::endl;

		std::exit(EXIT_FAILURE);
	}
}

/**
 * \brief Empty
 */
ConfigurationManager::~ConfigurationManager()
{
}

/**
 * \brief Retrieve a string value frome the configuration file
 * \param property The property (key) to read
 * \param def The value to return if no value for property is found
*/
std::string ConfigurationManager::getString(std::string property, std::string def)
{
	if (!m_json.HasMember(property.c_str())) return def;
	assert(m_json[property.c_str()].IsString());
	return m_json[property.c_str()].GetString();
}

/**
 * \brief Retrieve an integer value frome the configuration file
 * \param property The property (key) to read
 * \param def The value to return if no value for property is found
*/
int ConfigurationManager::getInt(std::string property, int def)
{
	if (!m_json.HasMember(property.c_str())) return def;
	assert(m_json[property.c_str()].IsInt());
	return m_json[property.c_str()].GetInt();
}

/**
 * \brief Retrieve a boolean value frome the configuration file
 * \param property The property (key) to read
 * \param def The value to return if no value for property is found
*/
bool ConfigurationManager::getBool(std::string property, bool def)
{
	if (!m_json.HasMember(property.c_str())) return def;
	assert(m_json[property.c_str()].IsBool());
	return m_json[property.c_str()].GetBool();
}

/**
 * \brief Retrieve a double value frome the configuration file
 * \param property The property (key) to read
 * \param def The value to return if no value for property is found
*/
double ConfigurationManager::getDouble(std::string property, double def)
{
	if (!m_json.HasMember(property.c_str())) return def;
	assert(m_json[property.c_str()].IsNumber());
	return m_json[property.c_str()].GetDouble();
}
