#include "../inc/Server.hpp"
# include <iostream>

Server::Server(void)
{
    std::cout << "Default server created" << std::endl;
}

Server::Server(const std::vector<ServerConfig> &configs)
{
    (void)configs;
    std::cout << "Server created using config file"<< std::endl;
}

Server::Server(const Server &other)
{
    (void)other;
    std::cout << "Server created as a copy" << std::endl;
}

Server &Server::operator=(const Server &other)
{
    (void)other;
    std::cout << "Server assigned as a copy" << std::endl;
    return (*this);
}

Server::~Server(void)
{
    std::cout << "Server destroyed" << std::endl;
}
