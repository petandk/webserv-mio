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
    if (file.bad()) return (false);
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

    if (this->_bufferTokens.empty()) return (false);
        
    return (true);
}

bool ConfigParser::parseTokens(void)
{
    this->_currentToken = 0;
    this->_parsedServerConfigs.clear();
    
    while (this->_currentToken < this->_bufferTokens.size())
    {
        ServerConfig server;
        if (!this->parseServerBlock(server)) return (false);
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
        if (!hasToken() || getToken() != token) return (false);
    this->_currentToken++;
    return (true);
}

bool ConfigParser::parseServerBlock(ServerConfig &server)
{
    const std::string serverDirectives[] ={
        "listen",
        "host",
        "server_name",
        "client_max_body_size",
        "root",
        "index",
        "error_page",
        "location"
    };
    size_t i;
    if (!consumeToken("server")) return (false);
    if (!consumeToken("{")) return (false);
    while (hasToken() && getToken() != "}")
    {
        i = 0;
        while (i < 8 && getToken() != serverDirectives[i])
            i++;
        switch(i)
        {
            case 0: if (!parseListen(server)) return (false); break;
            case 1: if (!parseHost(server)) return (false); break;
            case 2: if (!parseServerName(server)) return (false); break;
            case 3: if (!parseMaxBodySize(server)) return (false); break;
            case 4: if (!parseServerRoot(server)) return (false); break;
            case 5: if (!parseServerIndex(server)) return (false); break;
            case 6: if (!parseErrorPage(server)) return (false); break;
            case 7: if (!parseLocationBlock(server)) return (false); break;
            default: return (false);
        }
    }
    // default values here:
    if(server.getHost().empty())
        server.setHost("0.0.0.0");
    
    if(!consumeToken("}")) return (false);
    return (true);
}

bool ConfigParser::parseListen(ServerConfig &server)
{
    return parseNumbers(server, "listen", 1, 65535, &ServerConfig::addPort);
}

/*
 * %d.%d.%d.%d matches the 4 numbers.
 * %c (garbage) catches any extra characters at the end.
 * Returns 4 only if there is no extra junk after the IP.
 */

bool ConfigParser::parseHost(ServerConfig &server)
{
    if(!consumeToken("host") || !hasToken()) return (false);
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

bool ConfigParser::parseServerName(ServerConfig &server)
{
    return (parseString(server, "server_name", "*.-", &ServerConfig::addServerName));
}

bool ConfigParser::parseMaxBodySize(ServerConfig &server)
{
    return (parseNumbers<std::size_t>(server, "client_max_body_size", 0, ULONG_MAX, &ServerConfig::setClientMaxBodySize));
}

bool ConfigParser::parseServerRoot(ServerConfig &server)
{
    return (parseString(server, "root", "/._-", &ServerConfig::setRoot));
}

bool ConfigParser::parseServerIndex(ServerConfig &server)
{
    return (parseString(server, "index", "/._-", &ServerConfig::addIndexFile));
}

bool ConfigParser::parseErrorPage(ServerConfig &server)
{
    if (!consumeToken("error_page") || !hasToken())
        return (false);
    bool foundError = false;
    while (hasToken() && getToken() != ";")
    {
        std::string errorN = getToken();

        int code;
        if (!isAllDigits(errorN)) return false;
        std::stringstream ss(errorN);
        if (!(ss >> code) || code < 100 || code > 599)
            return false;
        if(!consumeToken(errorN)) return (false);
        std::string error = getToken();
        if(!::isAllowedChars(error, "/._-")) return (false);
        server.addErrorPage(code, error);
        foundError = true;
        if(!consumeToken(error)) return (false);
    }
    if (!foundError || !consumeToken(";")) return (false);
    return (true);
}

/*
    sacar el codigo de Location en parseLocation y hacer una
    template, luego llamarlo aqui con serverconfig y 
    dentro llamarlo con LocationConfig recursivo
    case 7: if (!parseLocation(location)) return (false); break;

*/

bool ConfigParser::parseLocationBlock(ServerConfig &server)
{
    const std::string locationDirectives[] = {
        "root",
        "index",
        "allowed_methods",
        "upload_path",
        "autoindex",
        "cgi_extension",
        "cgi_pass",
        "location"
    };
    size_t i;
    LocationConfig location;
    if (!consumeToken("location") || ! hasToken())
        return (false);
    std::string path = getToken();
    if (!parseString(location, path, "/.*_-~%@+", &LocationConfig::setPath))
        return (false);
    if (!consumeToken(path)) return (false);
    if (!consumeToken("{")) return(false);
    while (hasToken() && getToken() != "}")
    {
        i = 0;
        while (i < 8 && getToken() != locationDirectives[i])
            i++;
        switch(i)
        {
            case 0: if (!parseLocationRoot(location)) return (false); break;
            case 1: if (!parseLocationIndex(location)) return (false); break;
            case 2: if (!parseAllowedMethods(location)) return (false); break;
            case 3: if (!parseUploadPath(location)) return (false); break;
            case 4: if (!parseAutoindex(location)) return (false); break;
            case 5: if (!parseCGIExtension(location)) return (false); break;
            case 6: if (!parseCGIpass(location)) return (false); break;
            case 7: if (!parseLocation(location)) return (false); break;
            default: return (false);
        }
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
