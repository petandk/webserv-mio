#include "../inc/parser/ConfigParser.hpp"
#include "../inc/utils/Debug.hpp"
#include "../inc/server/Server.hpp"
#include <iostream>
#include <signal.h>

void goodByeHandler(int)
{
    std::cout << std::endl << "\033[1;32m[webserv] Server stopped. Goodbye!\033[0m" << std::endl;
}

/*
    valgrind will throw 'still reachable' leaks because the only way to kill
    the server is Ctrl+C it. Which ends without cleaning up or
    calling any class destructor.
*/
int main(int argc, char *argv[])
{
    signal(SIGINT, goodByeHandler);
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
