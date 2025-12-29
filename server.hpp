#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include "ServerConfig.hpp" // el hpp de Pau

class Request;      // La clase de jsayerza

class Server {
private:
    std::vector<struct pollfd>  _fds;
    std::vector<ServerConfig>   _configs; 
    std::map<int, std::string>  _clientBuffers;

    void setupSockets();           // socket(), bind(), listen()
    void acceptNewConnection();    // accept()
    void readFromClient(int fd);   // recv() -> Leer y guardar bytes
    void sendToClient(int fd);     // send() -> Envía la respuesta de Sayer

public:
        Server();                                   // 1. Constructor por defecto
        Server(const std::vector<ServerConfig> &configs); //constructor con config
        Server(const Server &other);                  // 2. Constructor de copia
        Server &operator=(const Server &other);       // 3. Operador de asignación
        ~Server();                                  // 4. Destructor

    void run();
};

#endif