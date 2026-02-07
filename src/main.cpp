#include "../inc/ConfigParser.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    ConfigParser parser;

    if(argc > 2)
    {
        std::cout << "⚠️Wrong usage!⚠️" << std::endl;
        std::cout << "executable should be executed as follows:" << std::endl;
        std::cout << "./webserv [configuration file]" << std::endl;
        return (1);
    }
    bool success;
    if(argc == 2)
        success = parser.parseConfigFile(argv[1]);
    else
        success = parser.parseConfigFile();
    if (success)
        std::cout << parser.getFileBuffer();

    return (1);
}
