#include "../inc/server/Server.hpp"
#include <iostream>
#include <sys/types.h> //System types (size_t, ssize_t...)
#include <sys/socket.h> //socket(), bind(), listen(), accept(), setsockopt(), sockaddr
#include <netinet/in.h> // sockaddr_in, htons(), htonl()
#include <fcntl.h> //To enable non-blocking mode fcntl() and O_NONBLOCK
#include <cstring> // memset()
#include <poll.h> //I/O multiplexing poll()
#include <unistd.h> //close for fd

bool Server::setupSockets(void)
{
    //first we go over all servers
    for (size_t i = 0; i < this->_allServers.size(); i++)
    {
        //now we have to go over all ports
    }
}

bool Server::isListening(int fd)
{
    for (size_t i = 0; i < this->_listenFds.size(); i++)
    {
        if (fd == this->_listenFds[i])
            return (true);
    }
    return (false);
}

std::string Server::getRawRequest(int client_fd) const
{
    std::map<int, std::string>::const_iterator it = this->_clientBuffers.find(client_fd);
    if (it != this->_clientBuffers.end())
        return (it->second);
    return ("");
}

void Server::kickClient(int fd)
{
    close(fd);
    for (size_t i = 0; i < this->_fds.size(); i++)
    {
        if (this->_fds[i].fd == fd)
        {
            this->_fds.erase(_fds.begin() + i);
            break;
        }
    }
    this->_clientBuffers.erase(fd);
    this->_lastActivity.erase(fd);
    #ifdef DEBUG
        std::cout << "[DEBUG] client with fd '" << fd << "' was kicked." << std::endl;
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

Server::Server(void)
{
    #ifdef DEBUG
        std::cout << "Default server created" << std::endl;
    #endif
}

Server::Server(const ConfigParser &configs):_allServers(configs.getParsedServerConfigs())
{
    #ifdef DEBUG
        std::cout << "Server created using config file" << std::endl;
    #endif
}

Server::Server(const Server &other)
    :_allServers(other._allServers),
    _fds(other._fds),
    _listenFds(other._listenFds),
    _clientBuffers(other._clientBuffers)
{
    #ifdef DEBUG
        std::cout << "Server created as a copy" << std::endl;
    #endif
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _allServers = other._allServers;
        _fds = other._fds;
        _listenFds = other._listenFds;
        _clientBuffers = other._clientBuffers;
    }
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

bool Server::run(void)
{
    if (!setupSockets())
        return (false);

    bool activeServer = true;
    while (activeServer)
    {
        time_t now = time(NULL);
        for (std::map<int, time_t>::iterator it = this->_lastActivity.begin(); it != this->_lastActivity.end(); )
        {
            if (now - it->second > IDLE_TIMEOUT / 1000)
            {
                kickClient(it->first);
                this->_lastActivity.erase(it++);
            }
            else
                it++;
        }
        int pollRes = poll(&this->_fds[0], this->_fds.size(), POLL_TIMEOUT);
        if (pollRes < 0)
        {
            activeServer = false;
            break;
        }
        if (pollRes == 0)
            continue;
        for (int i = this->_fds.size() - 1; i >= 0 ; i--)
        {
            if (this->_fds[i].revents == 0)
                continue;
            
            int fd = _fds[i].fd;
            
            if(isListening(fd))
            {
                if (!acceptNewConnection())
                    activeServer = false;
                continue;
            }
            // POLLRDHUP is kind of new and linux-specific so need to be protected 
            # ifdef POLLRDHUP
                if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP))
            # else
                if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
            # endif
            {
                kickClient(fd);
                continue;
            }
            if (_fds[i].revents & POLLIN)
            {
                if (!readFromClient(fd))
                {
                    kickClient(fd);
                    continue;
                }
                else
                    this->_lastActivity[fd] = time(NULL);
            }
            if (_fds[i].revents & POLLOUT)
            {
                if (!sendToClient(fd))
                {
                    kickClient(fd);
                    continue;
                }
                else
                    this->_lastActivity[fd] = time(NULL);
            }
        }
    }
    return (activeServer);
}