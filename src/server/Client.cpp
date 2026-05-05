#include "../../inc/server/Client.hpp"

Client::Client(void)
    :_fd(-1),
    _lastActivity(time(NULL)),
    _config(NULL)
{}

Client::Client(int fd, const ServerConfig *config)
    :_fd(fd),
    _lastActivity(time(NULL)),
    _config(config)
{}

Client::Client(const Client &other)
    :_fd(other._fd),
    _lastActivity(other._lastActivity),
    _requestBuffer(other._requestBuffer),
    _responseBuffer(other._responseBuffer),
    _config(other._config)
{}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        this->_fd = other._fd;
        this->_lastActivity = other._lastActivity;
        this->_requestBuffer = other._requestBuffer;
        this->_responseBuffer = other._responseBuffer;
        this->_config = other._config;
    }
}

Client::~Client(void)
{}

void Client::updateActivity(void)
{
    this->_lastActivity = time(NULL);
}

bool Client::hasTimedOut(int timeoutLimit) const
{
    return ((time(NULL) - this->_lastActivity) > timeoutLimit);
}

void Client::appendRequest(const char *data, ssize_t size)
{
    this->_requestBuffer.append(data, size);
}

void Client::setResponse(const std::string &response)
{
    this->_responseBuffer = response;
}

void Client::eraseSentResponse(size_t bytesSent)
{
    this->_responseBuffer.erase(0, bytesSent);
}

void Client::clearRequest(void)
{
    this->_requestBuffer.clear();
}

int Client::getFd(void) const
{
    return (this->_fd);
}

const std::string &Client::getRequestBuffer(void) const
{
    return (this->_requestBuffer);
}

const std::string &Client::getResponseBuffer(void) const
{
    return (this->_responseBuffer);
}

const ServerConfig *Client::getConfig(void) const
{
    return (this->_config);
}

/*
    recicla esto

    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0)
    {
        /*
            Since we set the O_NONBLOCK, 
            EWOULDBLOCK / EAGAIN: "No data available right now. Try again later."
            return true because is not a error, just nothing to say.
        *//*
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return (true);
        std::cout << "\033[1;31m[ERROR] recv() failed with fd: " << fd << "\033[0m" << std::endl;
        return (false);
    }
    else if (bytesRead == 0)
    {
        std::cout << "\033[93m[INFO] Conection closed with fd: " << fd << ")\033[0m" << std::endl;
        return false;
    }
    size_t currentSize = this->_clientBuffers[fd].size();
    size_t maxBody = it->second->getClientMaxBodySize();
    if (currentSize + bytesRead > maxBody)
    {
        std::cout << "\033[1;31m[ERROR] Request bigger than client_max_body_size ("
                    << maxBody << " bytes) with fd: " << fd << "\033[0m" << std::endl; 
        // error 413 here?
        return (false);
    }
    buffer[bytesRead] = '\0';
    this->_clientBuffers[fd].append(buffer, bytesRead);
    #ifdef DEBUG
        std::cout << "[DEBUG] Receibed" << bytesRead << "bytes from fd " << fd << std:endl;
    #endif
*/