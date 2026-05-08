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
        int                 _responseFd;
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
        
        void                setResponseFd(int fd);
        bool                hasResponse(void) const;
        
        void                clearRequest(void);

        int                 getFd(void) const;
        const std::string   &getRequestBuffer(void) const;
        int                 getResponseFd(void) const;
        const ServerConfig  *getConfig(void) const;
};

#endif