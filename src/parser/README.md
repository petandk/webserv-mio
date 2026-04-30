# webserv

## Configuration Parser Overview

This project implements a web server in C++98 with a configuration parser inspired by Nginx. The parser reads configuration files, validates them, and builds the internal server and location configuration objects.

### How the Parser Works
1. **Parameter conf file:** If no file is specified, `conf/Default.conf` is used.
2. **Preprocessing:** Comments and extra whitespace are removed.
3. **Tokenization:** The file is split into tokens (keywords, values, `{`, `}`, `;`).
4. **Parsing:** Tokens are processed to build `ServerConfig` and `LocationConfig` objects.
5. **Validation:** Syntax and value checks are performed. Errors stop the server with a clear message.

### Example Configuration (`conf/Default.conf`)
```nginx
server {
    listen 8080;
    host 127.0.0.1;
    server_name localhost;
    client_max_body_size 1048576;
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
    location / {
        root ./var/www/html;
        index index.html;
        allowed_methods GET POST;
        autoindex off;
    }
    location /uploads {
        root ./var/www/uploads;
        allowed_methods GET POST DELETE;
        upload_path ./var/www/uploads;
        autoindex on;
    }
    location /forbidden {
        return 404;
    }
    location /cgi-bin {
        root ./var/www/cgi-bin;
        allowed_methods GET POST;
        cgi_extension .py;
        cgi_pass /usr/bin/python3;
    }
}
```

### Example Output (Debug Mode)
```
No config file specified, using default configuration.
[DEBUG] Tokens after tokenization:
  [0]: 'server'
  [1]: '{'
  ...
[DEBUG] Calling parseServerBlock. Current token: 'server'
[DEBUG] Entering parseServerBlock
[DEBUG] Entering parseLocationBlock. Current token: 'location'
[DEBUG] Location added to server
...
=== Data after parsing ===
=== Server 0 ===
Host: 127.0.0.1
Ports: 8080
Server Names: localhost
Client max body size: 1048576
Error pages:
  404 -> /errors/404.html
  500 -> /errors/500.html
 --- Location 0 ---
  Path: /
  Root: ./var/www/html
  Index files: index.html
  Allowed methods: GET, POST
  Autoindex: off
  CGI extension: 
  CGI pass: 
  Redirection code: 0
  Redirect URL: 
 --- Location 1 ---
  Path: /uploads
  Root: ./var/www/uploads
  Index files: 
  Allowed methods: GET, POST, DELETE
  Upload path: ./var/www/uploads
  Autoindex: on
  CGI extension: 
  CGI pass: 
  Redirection code: 0
  Redirect URL: 
 --- Location 2 ---
  Path: /forbidden
  Root: 
  Index files: 
  Allowed methods: 
  Upload path: 
  Autoindex: off
  CGI extension: 
  CGI pass: 
  Redirection code: 404
  Redirect URL: 
 --- Location 3 ---
  Path: /cgi-bin
  Root: ./var/www/cgi-bin
  Index files: 
  Allowed methods: GET, POST
  Upload path: 
  Autoindex: off
  CGI extension: .py
  CGI pass: /usr/bin/python3
  Redirection code: 0
  Redirect URL: 
```

### Main Classes
- **ConfigParser:** Handles parsing, tokenization, and validation.
- **ServerConfig:** Stores server-level configuration.
- **LocationConfig:** Stores location-specific configuration.

### Error Handling
If the configuration file is missing, empty, or contains errors, the parser will print an error and the server will not start.

---

## Build & Run Instructions

### Compile (Release)
```
make
```

### Compile (Debug mode)
```
make debug
```

### Run with default configuration
```
./webserver
```
(This will use `conf/Default.conf` automatically if no argument is given.)

### Run with a specific configuration file
```
./webserver conf/Testing.conf
```
(Replace `conf/Testing.conf` with your desired config file path.)

### Clean object files
```
make clean
```

### Clean everything (objects + executable)
```
make fclean
```

### Rebuild everything from scratch
```
make re
```

---

For more details, see the code and comments in the source files.

---

## Default Values Assigned by the Parser

The configuration parser assigns default values to several directives if they are not specified in the configuration file. This ensures the server always has valid settings and simplifies configuration files. The inheritance rules are:

- **Server-level defaults:** If a directive is missing in a `server` block, the parser assigns the default value shown below.
- **Location-level inheritance:** If a `location` block omits `root` or `index`, it inherits the value from its parent `server` block. Other directives in `location` have their own defaults or remain unset unless specified.

| Directive                | Context   | Default Value         | Inheritance/Notes                                  |
|--------------------------|-----------|----------------------|----------------------------------------------------|
| `host`                   | server    | `0.0.0.0`            | Used if not specified in server block              |
| `listen` (port)          | server    | `8080`               | Used if not specified in server block              |
| `root`                   | server    | `./var/www/html`     | Inherited by location if not set                   |
| `index`                  | server    | `index.html`         | Inherited by location if not set                   |
| `client_max_body_size`   | server    | `1048576` (1 MB)     | Used if not specified in server block              |
| `server_name`            | server    | `localhost`          | Used if not specified in server block              |
| `error_page`             | server    | *none*               | No default error pages                             |
| `allowed_methods`        | location  | `GET`                | Only GET allowed if not specified                  |
| `autoindex`              | location  | `off`                | Directory listing disabled by default              |
| `upload_path`            | location  | *none*               | Uploads disabled unless specified                  |
| `cgi_extension`          | location  | *none*               | CGI disabled unless specified                      |
| `cgi_pass`               | location  | *none*               | CGI disabled unless specified                      |
| `return` (redirect code) | location  | `0` (no redirect)    | No redirection unless specified                    |

**Examples:**

- If a `server` block does not specify `host`, it will listen on `0.0.0.0`.
- If a `location` does not specify `root`, it will use the `root` from its parent `server`.
- If `allowed_methods` is not set in a `location`, only `GET` will be allowed.

**Note:** The parser ensures that every server and location has valid values for all required directives, either from the config file, by inheritance, or by default.
