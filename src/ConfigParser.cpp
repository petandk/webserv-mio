#include "../inc/ConfigParser.hpp"
#include "../inc/Utils.hpp"
#include <iostream>
#include <fstream>

ConfigParser::ConfigParser(void)
{
}

ConfigParser::ConfigParser(const ConfigParser &other)
{
    this->_parsedServerConfigs = other._parsedServerConfigs;
    this->_fileBuffer = other._fileBuffer;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
    if (this != &other)
    {
        this->_parsedServerConfigs = other._parsedServerConfigs;
        this->_fileBuffer = other._fileBuffer;
    }
    return (*this);
}

ConfigParser::~ConfigParser(void)
{
}

bool ConfigParser::parseConfigFile(void)
{
    std::cout << "No config File specified, using default configuration." << std::endl;
    return (parseConfigFile("Default.conf"));
}

/*

 */
bool ConfigParser::parseConfigFile(const std::string &configFile)
{
    try
    {
        std::string fullPath = "conf/" + configFile;
        std::ifstream file(fullPath.c_str());
        std::string line;

        if (!file.is_open())
            throw ConfFileException();

        this->_fileBuffer.clear();
        while (std::getline(file, line)) {
            RemoveComments(line);
            trimWhitepaces(line);
            if (!line.empty())
            {
                this->_fileBuffer += line + "\n";
                #ifdef DEBUG
                    std::cout << line << std::endl;
                #endif
            }
        }
        file.close();
        return (true);
    }
    catch (const std::exception &e)
    {
        std::cerr <<e.what() << std::endl;
        return (false);
    }
}

const std::string   &ConfigParser::getFileBuffer(void)
{
    return (this->_fileBuffer);
}

const std::vector<ServerConfig> &ConfigParser::getParsedServerConfigs(void) const
{
    return this->_parsedServerConfigs;
}

//exceptions
const char *ConfigParser::ConfFileException::what() const throw()
{
    return("Error: Cannot open/read the configuration file or file is empty.");
}

const char *ConfigParser::ConfSyntaxException::what() const throw()
{
    return("Error: Invalid syntax in configuration file.");
}

const char *ConfigParser::ConfValueException::what() const throw()
{
    return("Error: Invalid value for directive in configuration file.");
}

const char *ConfigParser::ConfDirectiveException::what() const throw()
{
    return("Error: Missing required directive in configuration file.");
}
