#include "../inc/parser/ConfigParser.hpp"
#include "../inc/utils/Debug.hpp"
#include "../inc/server/Server.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc > 2)
    {
        std::cout << "⚠️Wrong usage!⚠️" << std::endl;
        std::cout << "executable should be executed as follows:" << std::endl;
        std::cout << "./webserv [configuration file]" << std::endl;
        return (1);
    }
    ConfigParser configs;
    bool success = false;
    if(argc == 2)
        success = configs.parseConfigFile(argv[1]);
    else
        success = configs.parseConfigFile();

    #ifdef DEBUG
    if (success) {
        printParsedConfig(configs);
    }
    #endif
    if (success)
    {
        Server webserv(configs);
        success = webserv.run();
    }

    /*
        !success because true = 1, false = 0
        but return expects 0 for no error, anything else for error
    */
    return (!success);
}
