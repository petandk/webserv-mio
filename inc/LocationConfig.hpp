#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
    private:
        std::string _path;
        std::string _root;
        std::vector<std::string> _allowedMethods;
        bool _autoindex;
        std::string _indexFile;
        std::string _cgiExtension;
        std::string _cgiPass;
        std::string _uploadPath;
        int _redirectCode;
        std::string _redirectUrl;

    public:
        LocationConfig(void);
        LocationConfig(const LocationConfig &other);
        LocationConfig &operator=(const LocationConfig &other);
        ~LocationConfig(void);

        const std::string &getPath(void) const;
        const std::string &getRoot(void) const;
        const std::vector<std::string> &getAllowedMethods(void) const;
        bool getAutoindex(void) const;
        const std::string &getIndexFile(void) const;
        const std::string &getCgiExtension(void) const;
        const std::string &getCgiPass(void) const;
        const std::string &getUploadPath(void) const;
        int getRedirectCode(void) const;
        const std::string &getRedirectUrl(void) const;

        void setPath(const std::string &path);
        void setRoot(const std::string &path);
        void setAllowedMethods(const std::vector <std::string> &methods);
        void setAutoindex(bool state);
        void setIndexFile(const std::string &path);
        void setCgiExtension(const std::string &extension);
        void setCgiPass(const std::string &path);
        void setUploadPath(const std::string &path);
        void setRedirectionCode(int code);
        void setRedirectUrl(const std::string &path);
};

#endif
