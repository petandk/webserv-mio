#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <ctime>
# include "../parser/ServerConfig.hpp"

class Client {
    private:
        int                 _fd;
        time_t              _lastActivity;
        std::string         _requestBuffer;
        std::string         _responseBuffer;
        const ServerConfig *_config;

    public:
        Client(void);
        Client(int fd, const ServerConfig *config);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client(void);

        void                updateActivity(void);
        bool                hasTimedOut(int timeoutLimit) const;
        void                appendRequest(const char *data, ssize_t size);
        
        void                setResponse(const std::string &response);
        void                eraseSentResponse(size_t bytesSent);
        void                clearRequest(void);

        int                 getFd(void) const;
        const std::string   &getRequestBuffer(void) const;
        const std::string   &getResponseBuffer(void) const;
        const ServerConfig  *getConfig(void) const;
};

#endif