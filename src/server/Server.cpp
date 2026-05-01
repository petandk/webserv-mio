#include "../inc/server/Server.hpp"
#include <iostream>
#include <sys/types.h> //System types (size_t, ssize_t...)
#include <sys/socket.h> //socket(), bind(), listen(), accept(), setsockopt(), sockaddr
#include <netinet/in.h> // sockaddr_in, htons(), htonl()
#include <fcntl.h> //To enable non-blocking mode fcntl() and O_NONBLOCK
#include <cstring> // memset()
#include <poll.h> //I/O multiplexing poll()
#include <unistd.h> //close for fd
#include <netdb.h> //getaddrinfo, freeaddrinfo
#include <sstream> //to turn int to string

void printServerInfo(std::string &host, int port, const std::vector<std::string> &names)
{

    std::cout << std::endl << "\033[96m--- [SERVER] listening on ---\033[0m" << std::endl
    << "-IP/HOST: " << host << std::endl
    << "-PORT: " << port << std::endl;
    bool validName = false;
            for(size_t n = 0; n < names.size(); n++)
            {
                if (!names[n].empty())
                {
                    if (!validName)
                    {
                        std::cout << "-SERVER NAME(S): " << std::endl;
                        validName = true;
                    }
                    std::cout << "\t" << names[n] << std::endl;
                }
            }
}

bool Server::setupSockets(void)
{
    std::vector<std::string>    openedHosts;
    std::vector<int>            openedPorts;

    //first we go over all servers
    for (size_t i = 0; i < this->_allServers.size(); i++)
    {
        std::string host = this->_allServers[i].getHost();
        const std::vector<int> &ports = this->_allServers[i].getPorts();

        //now we have to go over all ports
        for (size_t p = 0; p < ports.size(); p++)
        {
            int port = ports[p];

            bool isOpened = false;
            for (size_t j = 0; j < openedPorts.size(); j++)
            {
                if (openedHosts[j] == host && openedPorts[j] == port)
                {
                     isOpened = true;
                    break;
                }
            }
            if (isOpened)
                continue;
            int listenFd = socket(AF_INET, SOCK_STREAM, 0);
            if (listenFd < 0)
            {
                std::cout << "\033[1;31m[ERROR] Socket creation failed.\033[0m" << std::endl;
                return (false);
            }
            
            int opt = 1;
            if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            {
                #ifdef DEBUG
                    std::cout << "[DEBUG-SERVER] Socket config failed." << std::endl;
                #endif
                close(listenFd);
                return (false);
            }
            if (fcntl(listenFd, F_SETFL,O_NONBLOCK) < 0)
            {
                std::cout << "\033[1;31m[ERROR] fcntl failed to set NONBLOCK.\033[0m" << std::endl;
                close(listenFd);
                return (false);
            }
            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype  = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;

            std::stringstream ss;
            ss << port;
            std::string portStr = ss.str();

            if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res) != 0)
            {
                std::cout << "\033[1;31m[ERROR] getaddrinfo failed for host '" << host << "' and port '" << port << "'.\033[0m" << std::endl;
                close(listenFd);
                return (false);
            }

            if (bind(listenFd, res->ai_addr, res->ai_addrlen) < 0)
            {
                std::cout << "\033[1;31m[ERROR] Port " << port << " on host '" << host
                << "' is already in use or cannot be bound.\n\tMake sure no other server is running on this port and que la IP existe.\033[0m" << std::endl;
                freeaddrinfo(res);
                close(listenFd);
                return (false);
            }

            freeaddrinfo(res);

            if (listen(listenFd, SOMAXCONN) < 0)
            {
                std::cout << "\033[1;31m[ERROR] listen() failed.\033[0m" << std::endl;
                close(listenFd);
                return (false);
            }

            struct pollfd pfd;
            pfd.fd = listenFd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            this->_fds.push_back(pfd);
            this->_listenFds.push_back(listenFd);

            openedHosts.push_back(host);
            openedPorts.push_back(port);

            const std::vector<std::string> &names = this->_allServers[i].getServerNames();
            printServerInfo(host, port, names);
            
        }

    }
    return (!this->_listenFds.empty()); 
}

bool Server::acceptNewConnection(void)
{
    return (true);
}

bool Server::readFromClient(int fd)
{
    (void)fd;
    return (true);
}

bool Server::sendToClient(int fd)
{
    (void)fd;
    return (true);
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
    _clientBuffers(other._clientBuffers),
    _listenFds(other._listenFds)

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
        std::cout << "\033[1;32mServer successfully destroyed!\033[0m" << std::endl;
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