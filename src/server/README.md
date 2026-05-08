# Server Module - Network Engine

## Overview
This module implements the **core network engine** of the web server. It is responsible for managing all TCP connections, handling asynchronous I/O using `poll()`, and coordinating the communication between clients and the HTTP handler. The design follows a **non-blocking, event-driven architecture** inspired by Nginx.

---

## 🛠 Architecture & Data Flow

### Core Components
- **`Server`**: The main class that owns the event loop, socket management, and client lifecycle.
- **`Client`**: Represents a single TCP connection with its state, request buffer, and response file descriptor.
- **`HttpHandler`**: External component (implemented by my teammate) that processes HTTP requests and generates responses.

### Data Flow Diagram
```
[Network] → [poll()] → [Event Detected]
                              │
                    ┌─────────┴─────────┐
                    │                   │
               POLLIN              POLLOUT
                    │                   │
            readFromClient()     sendToClient()
                    │                   │
            Request Buffer      Read from responseFd
                    │                   │
            \r\n\r\n found?       Send to socket
                    │
            HttpHandler.handleRequest()
                    │
            client.setResponseFd(fd)
```

### The Contract with HttpHandler
The Server module communicates with the HTTP handler through a **minimal interface**:

| Server provides | HttpHandler provides |
|----------------|---------------------|
| `Client::getRequestBuffer()` - Raw HTTP request | `Client::setResponseFd(int fd)` - FD with full response |
| `Client::getConfig()` - Server configuration | The FD contains: headers + body (complete HTTP response) |

> **Key Design Decision**: The response FD contains the **entire HTTP response** (headers + body). The Server's `sendToClient()` simply reads from this FD and sends to the socket. This unifies handling of static files and CGI.

---

## 📦 Class Reference

### Client Class

#### Member Variables
| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `_fd` | `int` | `-1` | Client socket file descriptor |
| `_lastActivity` | `time_t` | `time(NULL)` | Timestamp for timeout detection |
| `_requestBuffer` | `std::string` | empty | Accumulated HTTP request data |
| `_responseFd` | `int` | `-1` | FD with complete HTTP response |
| `_config` | `const ServerConfig*` | `NULL` | Pointer to associated server config |

#### Main Methods
| Method | Description |
|--------|-------------|
| `appendRequest(data, size)` | Appends received data to request buffer |
| `setResponseFd(fd)` | Sets the FD for response (called by HttpHandler) |
| `hasResponse()` | Returns `true` if `_responseFd >= 0` |
| `clearRequest()` | Clears request buffer (for keep-alive) |

### Server Class

#### Member Variables
| Variable | Type | Description |
|----------|------|-------------|
| `_allServers` | `std::vector<ServerConfig>` | Parsed server configurations |
| `_fds` | `std::vector<struct pollfd>` | File descriptors monitored by poll() |
| `_listenFds` | `std::vector<int>` | Listening socket FDs |
| `_clients` | `std::map<int, Client>` | Active client connections (fd → Client) |

#### Main Methods
| Method | Description |
|--------|-------------|
| `setupSockets()` | Creates and binds listening sockets |
| `acceptNewConnection(fd)` | Accepts new TCP connection |
| `readFromClient(fd)` | Reads data, detects complete requests |
| `sendToClient(fd)` | Sends response from FD to socket |
| `kickClient(fd)` | Removes client and cleans resources |
| `checkTimeouts()` | Removes idle clients |
| `run()` | Main event loop |

---

## 🔄 Event Loop Details

### The poll() Cycle
```
1. checkTimeouts() - Remove stale connections
2. poll(fds, nfds, TIMEOUT) - Wait for events
3. For each fd with events:
   ├── Listening FD → acceptNewConnection()
   ├── Error/EOF   → kickClient()
   ├── POLLIN      → readFromClient()
   └── POLLOUT     → sendToClient()
```

### State Machine (implicit in Client)
```
ACCEPTED → LEYENDO_PETICION → \r\n\r\n detected
           → HttpHandler processes → POLLOUT set
           → ENVIANDO_RESPUESTA → response complete
           → LEYENDO_PETICION (keep-alive) or CLOSED
```

### Event Transition Examples
```cpp
// After reading complete request with response ready
setClientEvents(fd, POLLOUT);  // Switch to write mode

// After sending complete response
setClientEvents(fd, POLLIN);   // Switch back to read mode (keep-alive)
```

---

## ⚙️ Configuration & Constants

### Server Constants (defined in `Server.hpp`)
| Constant | Value | Description |
|----------|-------|-------------|
| `POLL_TIMEOUT` | `1000` ms | poll() timeout period |
| `IDLE_TIMEOUT` | `30000` ms | Max idle time before kick |
| `READ_TIMEOUT` | `5000` ms | Max time for complex reads |
| `KEEP_TIMEOUT` | `15000` ms | Keep-alive timeout |
| `READ_BUFFER` | `4096` bytes | Buffer size for recv/read |

---

## 🚀 Build & Run

### Compilation
- **Standard build:** `make`
- **Debug mode:** `make debug` (Enables verbose connection logging)

### Execution
- **Default config:** `./webserver` (Uses `conf/Default.conf`)
- **Custom config:** `./webserver path/to/config.conf`

---

## 📝 Technical Details

### Non-blocking I/O
All sockets are set to non-blocking mode using `fcntl(fd, F_SETFL, O_NONBLOCK)`. This ensures:
- `accept()` never blocks the event loop
- `recv()` and `send()` return immediately with `EAGAIN` if not ready
- Single-threaded efficiency with many concurrent connections

### Memory Management
- **Request buffer**: Grows dynamically up to `client_max_body_size`
- **Response buffer**: No buffer needed - reads directly from FD in 4KB chunks
- **No copies**: Data flows from FD → 4KB buffer → socket without intermediate copies

### Error Handling
- **Socket errors**: `POLLERR`, `POLLHUP`, `POLLNVAL` → immediate `kickClient()`
- **EAGAIN/EWOULDBLOCK**: Non-fatal, retry on next poll cycle
- **Connection close**: `recv()` returns 0 → clean client removal
- **Timeout**: `checkTimeouts()` removes clients inactive for `IDLE_TIMEOUT`

### Keep-Alive Support
After sending a complete response, the server:
1. Closes the response FD
2. Clears the request buffer
3. Sets events back to `POLLIN`
4. Waits for the next request on the same connection

---

## 🔌 Integration with HttpHandler

### What the Server Expects
```cpp
class HttpHandler {
public:
    /**
     * @brief Process a complete HTTP request
     * @param client Reference to the client with request data
     * 
     * The handler must:
     * 1. Parse client.getRequestBuffer()
     * 2. Determine response (static file or CGI)
     * 3. Create FD with complete HTTP response
     * 4. Call client.setResponseFd(fd)
     */
    void handleRequest(Client& client);
};
```

### The Response FD Contract
The file descriptor set by `HttpHandler` must contain:
```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 1234\r\n
\r\n
<html>...body content...</html>
```

For static files: FD pointing to the file
For CGI: FD from pipe() after script execution
For errors: FD with error page content

---

## 🐛 Debug Mode Output

When compiled with `make debug`, the server provides detailed logging:

```text
[DEBUG] Server created using config file
[DEBUG-SERVER] Socket config failed.

--- [SERVER] listening on ---
-IP/HOST: 127.0.0.1
-PORT: 8080
-SERVER NAME(S):
    localhost

[92m-New client-
fd: 6
    IP:127.0.0.1
    PORT: 54321[0m

[DEBUG] Received 45 bytes from fd 6
[DEBUG] Request complete for fd 6
[DEBUG] Response set for fd 6 (headers: 156 bytes, body_fd: 7)
[DEBUG] Sent 4096 bytes to fd 6
[DEBUG] Response fully sent to fd 6
[DEBUG] client with fd '6' was kicked.
```

---

## 🧪 Testing

### Manual Testing with curl
```bash
# Basic request
curl -v http://localhost:8080/

# Request with body
curl -X POST -d "data=test" http://localhost:8080/upload

# Test keep-alive
curl -v --keepalive http://localhost:8080/

# Large file download
curl -o largefile.bin http://localhost:8080/large.bin
```

### Testing with telnet
```bash
telnet localhost 8080
GET / HTTP/1.1
Host: localhost
Connection: close

```

---

## ✅ Features Summary

- [x] Non-blocking I/O with poll()
- [x] Multiple server blocks (virtual hosts)
- [x] Client timeout detection
- [x] Keep-alive connections
- [x] Configurable max body size
- [x] Debug logging mode
- [x] Memory-efficient streaming (no full buffering)
- [x] Clean resource management
- [x] Error handling for all socket operations

---

## 📁 File Structure
```
src/
├── server/
│   ├── Server.cpp      # Main server logic & event loop
│   ├── Server.hpp      # Server class declaration
│   ├── Client.cpp      # Client state management
│   └── Client.hpp      # Client class declaration
│
handler/                 # (External - by teammate)
├── HttpHandler.cpp     # HTTP request processing
└── HttpHandler.hpp     # Handler interface
```

---
*For detailed source code comments, refer to the files in `src/server/` directory.*
