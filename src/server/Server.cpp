#include "../inc/server/Server.hpp"
#include <iostream>

Server::Server(void)
{
    #ifdef DEBUG
        std::cout << "Default server created" << std::endl;
    #endif
}

Server::Server(const ServerConfig &configs):_config(configs)
{
    #ifdef DEBUG
        std::cout << "Server created using config file" << std::endl;
    #endif
}

Server::Server(const Server &other)
{
    (void)other;
    #ifdef DEBUG
        std::cout << "Server created as a copy" << std::endl;
    #endif
}

Server &Server::operator=(const Server &other)
{
    (void)other;
    #ifdef DEBUG
        std::cout << "Server assigned as a copy" << std::endl;
    #endif
    return (*this);
}

Server::~Server(void)
{
    cleanup();
    #ifdef DEBUG
        std::cout << "Server destroyed and resources cleaned!" << std::endl;
    #endif
}

void Server::cleanup(void)
{
    for (size_t i = 0; i < this->_fds.size(); i++)
    {
        if (this->_fds[i].fd >= 0)
            close(this->_fds[i].fd);
    }
    this->_fds.clear();
    this->_clientBuffers.clear();
    #ifdef DEBUG
        std::cout << "\033[31mAll resources cleaned!\033[0m" << std::endl;
    #endif
}