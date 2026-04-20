#include "../inc/ConfigParser.hpp"
#include "../inc/Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <cstdio>

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
    #ifdef DEBUG
        std::cout << "No config file specified, using default configuration." << std::endl;
    #endif
    return (parseConfigFile("Default.conf"));
}

/*
    Main Parsing Function
 */
bool ConfigParser::parseConfigFile(const std::string &configFile)
{
    try
    {
        std::string fullPath = "conf/" + configFile;
        std::ifstream file(fullPath.c_str());

        if (!file.is_open())
            throw ConfFileException();
        if(!this->fillBuffer(file))
            throw ConfFileException();
        if (this->_fileBuffer.empty())
            throw ConfFileException();
        file.close();

        if (!this->tokenizeBuffer())
            throw ConfSyntaxException();
        if (!this->parseTokens())
            throw ConfSyntaxException();
        return (true);
    }
    catch (const std::exception &e)
    {
        std::cerr <<e.what() << std::endl;
        return (false);
    }
}

bool ConfigParser::fillBuffer(std::ifstream &file)
{
    std::string line;
    this->_fileBuffer.clear();
        while (std::getline(file, line)) {
            ::RemoveComments(line);
            ::trimWhitepaces(line);
            if (!line.empty())
                this->_fileBuffer += line + "\n";
        }
    if (file.bad())
        return (false);
    return (true);
}

bool ConfigParser::tokenizeBuffer(void)
{
    std::string current;
    char c;

    this->_bufferTokens.clear();
    for ( size_t i = 0; i < this->_fileBuffer.size(); i++)
    {
        c = this->_fileBuffer[i];
        if(c == ' ' || c =='\n'  || c =='\t' || c =='\r')
        {
            if (!current.empty())
                this->_bufferTokens.push_back(current);
            current.clear();
            continue;
        }
        else if(c == '{' || c == '}' || c == ';')
        {
            if (!current.empty())
                this->_bufferTokens.push_back(current);
            current.clear();
            this->_bufferTokens.push_back(std::string(1, c));
            continue;
        }

        else
            current.push_back(c);
    }
    if (!current.empty())
        this->_bufferTokens.push_back(current);

    if (this->_bufferTokens.empty())
        return (false);
        
    return (true);
}

bool ConfigParser::parseTokens(void)
{
    this->_currentToken = 0;
    this->_parsedServerConfigs.clear();
    
    while (this->_currentToken < this->_bufferTokens.size())
    {
        ServerConfig server;
        if (!this->parseServerBlock(server))
            return (false);
        this->_parsedServerConfigs.push_back(server);
    }
    return (true);
}

bool ConfigParser::hasToken(void) const
{
    return(this->_currentToken < this->_bufferTokens.size());
}

const std::string &ConfigParser::getToken(void) const
{
    return (this->_bufferTokens[this->_currentToken]);
}

bool ConfigParser::consumeToken(const std::string &token)
{
        if (!hasToken() || getToken() != token)
            return (false);
    this->_currentToken++;
    return (true);
}

bool ConfigParser::parseServerBlock(ServerConfig &server)
{
    const std::string serverDirectives[] ={
        "listen",
        "host",
        "server_name",
        "root",
        "index",
        "error_page",
        "location"
    };
    size_t i;
    if (!consumeToken("server"))
        return (false);
    if (!consumeToken("{"))
        return (false);
    while (hasToken() && getToken() != "}")
    {
        i = 0;
        while (i < 6 && getToken() != serverDirectives[i])
            i++;
        switch(i)
        {
            case 0: if (!parseListen(server)) return false; break;
            case 1: if (!parseHost(server)) return false; break;
            case 2: if (!parseServerName(server)) return false; break;
            case 3: if (!parseServerRoot(server)) return false; break;
            case 4: if (!parseServerIndex(server)) return false; break;
            case 5: if (!parseErrorPage(server)) return false; break;
            case 6: if (!parseLocationBlock(server)) return false; break;
            default: return false;
        }

    }
}

bool ConfigParser::parseListen(ServerConfig &server)
{
    if(!consumeToken("listen"))
        return (false);
    bool foundPort = false;
    while (hasToken() && getToken() != ";")
    {
        std::stringstream ss(getToken());
        int port;
        if (!(ss >> port) || port < 1 || port > 65535)
            return (false);
        if (getToken().length() != digitCounter(port))
            return (false);
        server.addPort(port);
        foundPort = true;
        if(!consumeToken(getToken()))
            return (false);
    }
    if (!foundPort || !consumeToken(";"))
        return (false);
    return (true);
}

/*

*/

bool ConfigParser::parseHost(ServerConfig &server)
{
    if(!consumeToken("host") || !hasToken())
        return (false);
    std::string host = getToken();
    int a,b,c,d;
    char garbage;
    if (std::sscanf(host.c_str(), "%d.%d.%d.%d%c", &a, &b, &c, &d, &garbage) != 4)
        return (false);
    if (a < 0 || a > 255 ||
        b < 0 || b > 255 ||
        c < 0 || c > 255 ||
        d < 0 || d > 255)
        return (false);
    server.setHost(host);
    if (!consumeToken(host) || !consumeToken(";"))
        return (false);
    return (true);
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
