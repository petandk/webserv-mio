#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <exception>
#include "ServerConfig.hpp"

class ConfigParser{
    private:
        std::vector<ServerConfig>   _parsedServerConfigs;
        std::string                 _fileBuffer;
    public:
        ConfigParser(void);
        ConfigParser(const ConfigParser &other);
        ConfigParser &operator=(const ConfigParser &other);
        ~ConfigParser(void);

        bool parseConfigFile(const std::string &configFile);

        const std::vector<ServerConfig> &getParsedServerConfigs(void) const;

        //exceptions
        class ConfFileException: public std::exception{
            public:
                virtual const char *what() const throw();
        };
        class ConfSyntaxException: public std::exception{
            public:
                virtual const char *what() const throw();
        };
        class ConfValueException: public std::exception{
            public:
                virtual const char *what() const throw();
        };
        class ConfDirectiveException: public std::exception{
            public:
                virtual const char *what() const throw();
        };
};

#endif
