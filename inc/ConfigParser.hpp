#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include "ServerConfig.hpp"

class ConfigParser{
    private:
        std::vector<ServerConfig>   _parsedServerConfigs;
        std::string                 _fileBuffer;
    public:
        ConfigParser(void);
        ConfigParser(const std::string &config_file);
        ConfigParser(const ConfigParser &other);
        ConfigParser &operator=(const ConfigParser &other);
        ~ConfigParser(void);

        bool parseConfigFile(const std::string &config_file);

        const std::vector<ServerConfig> &getParsedServerConfigs(void) const;
};

#endif
