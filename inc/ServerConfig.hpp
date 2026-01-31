#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <cstddef>
# include "LocationConfig.hpp"

class ServerConfig{
    private:
        std::vector<int> _ports;
        std::string _host;
        std::vector<std::string> _server_names;
        std::map<int, std::string> _errorPages;

    public:
        ServerConfig(void);
        ServerConfig(const ServerConfig &other);
        ServerConfig &operator=(const ServerConfig &other);
        ~ServerConfig(void);

};

#endif
