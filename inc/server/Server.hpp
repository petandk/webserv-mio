#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include <unistd.h>

# include "../parser/ServerConfig.hpp"

class Request;

class Server {
    private:
        std::vector<struct pollfd>  _fds;
        std::map<int, std::string>  _clientBuffers;
        ServerConfig                _config;
        std::vector<int>            _listen_fds;

        void setupSockets(void);           // socket(), bind(), listen()
        void acceptNewConnection(void);    // accept()
        void readFromClient(int fd);   // recv() -> Read and store bytes
        void sendToClient(int fd);     // send() -> Sends response to Protocol Handler

        std::string  getRawRequest(int client_fd) const; //Returns the content of _clientBuffers[client_fd]

        void cleanup(void);
    public:
        Server(void);
        Server(const ServerConfig &configs);
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server(void);

        void run(void);

        //TODO: exception classes here
};

#endif
