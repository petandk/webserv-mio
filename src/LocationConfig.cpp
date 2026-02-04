#include "../inc/LocationConfig.hpp"

//_allowedMethod is empty by default so no default init needed because is a vector
LocationConfig::LocationConfig(void)
{
    this->_path = "/";
    this->_root = "./var/www/html";
    this->_autoindex = false;
    this->_indexFile = "index.html";
    this->_cgiExtension = "";
    this->_cgiPass = "";
    this->_uploadPath = "";
    this->_redirectCode = 0;
    this->_redirectUrl = "";
}

LocationConfig::LocationConfig(const LocationConfig &other)
{
    this->_path = other._path;
    this->_root = other._root;
    this->_allowedMethods = other._allowedMethods;
    this->_autoindex = other._autoindex;
    this->_indexFile = other._indexFile;
    this->_cgiExtension = other._cgiExtension;
    this->_cgiPass = other._cgiPass;
    this->_uploadPath = other._uploadPath;
    this->_redirectCode = other._redirectCode;
    this->_redirectUrl = other._redirectUrl;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other)
{
    if (this != &other)
    {
        this->_path = other._path;
        this->_root = other._root;
        this->_allowedMethods = other._allowedMethods;
        this->_autoindex = other._autoindex;
        this->_indexFile = other._indexFile;
        this->_cgiExtension = other._cgiExtension;
        this->_cgiPass = other._cgiPass;
        this->_uploadPath = other._uploadPath;
        this->_redirectCode = other._redirectCode;
        this->_redirectUrl = other._redirectUrl;
    }
    return (*this);
}

LocationConfig::~LocationConfig(void)
{
}

const std::string &LocationConfig::getPath(void) const
{
    return(this->_path);
}

const std::string &LocationConfig::getRoot(void) const
{
    return(this->_root);
}

const std::vector<std::string> &LocationConfig::getAllowedMethods(void) const
{
    return(this->_allowedMethods);
}

bool LocationConfig::getAutoindex(void) const
{
    return (this->_autoindex);
}

const std::string &LocationConfig::getIndexFile(void) const
{
    return (this->_indexFile);
}

const std::string &LocationConfig::getCgiExtension(void) const
{
    return (this->_cgiExtension);
}

const std::string &LocationConfig::getCgiPass(void) const
{
    return (this->_cgiPass);
}

const std::string &LocationConfig::getUploadPath(void) const
{
    return (this->_uploadPath);
}

int   LocationConfig::getRedirectCode(void) const
{
    return (this->_redirectCode);
}

const std::string &LocationConfig::getRedirectUrl(void) const
{
    return (this->_redirectUrl);
}

void LocationConfig::setPath(const std::string &path)
{
    this->_path = path;
}
void LocationConfig::setRoot(const std::string &path)
{
    this->_root = path;
}
void LocationConfig::setAllowedMethods(const std::vector <std::string> &methods)
{
    this->_allowedMethods = methods;
}
void LocationConfig::setAutoindex(bool state)
{
    this->_autoindex = state;
}
void LocationConfig::setIndexFile(const std::string &path)
{
    this->_indexFile = path;
}
void LocationConfig::setCgiExtension(const std::string &extension)
{
    this->_cgiExtension = extension;
}
void LocationConfig::setCgiPass(const std::string &path)
{
    this->_cgiPass = path;
}
void LocationConfig::setUploadPath(const std::string &path)
{
    this->_uploadPath = path;
}
void LocationConfig::setRedirectionCode(int code)
{
    this->_redirectCode = code;
}
void LocationConfig::setRedirectUrl(const std::string &path)
{
    this->_redirectUrl = path;
}

void LocationConfig::addAllowedMethod(std::string &method)
{
    this->_allowedMethods.push_back(method);
}
