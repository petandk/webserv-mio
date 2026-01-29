#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include <poll.h>

# include "ServerConfig.hpp"

class Request;

class Server {
    private:
        std::vector<struct pollfd>  _fds;
        std::vector<ServerConfig>   _configs;
        std::map<int, std::string>  _clientBuffers;

        void setupSockets();           // socket(), bind(), listen()
        void acceptNewConnection();    // accept()
        void readFromClient(int fd);   // recv() -> Leer y guardar bytes
        void sendToClient(int fd);     // send() -> Envía la respuesta de Sayer

        const ServerConfig *getServerConfigByPort(int port) const;
        std::string  getRawRequest(int client_fd) const; //Devuelve el contenido de _clientBuffers[client_fd]

    public:
        Server(void);                                 // 1. Constructor por defecto
        Server(const std::vector<ServerConfig> &configs);          //1.1 Constructor con ruta
        Server(const Server &other);              // 2. Constructor de copia
        Server &operator=(const Server &other);   // 3. Operador de asignación
        ~Server(void);                                // 4. Destructor

        void run();

        //TODO: exception classes here
};

#endif
