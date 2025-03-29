#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>

// path of this file: Includes/Config/route.hpp


class route {

    private:
        // both
        std::vector<std::string>                        _allowed_methods;
        std::string							            _root;
        // route specific
        std::pair<std::size_t,std::string>	            _redirect; // for return
        bool								            _autoindex;
        std::string             			            _index;
        std::string                                     _alias;
        std::string                                     _actual_path;
        std::unordered_map<std::string, std::string>    _cgi;
        bool                                            _routeIsRedirect;

    public:
        route();
        ~route();

        void PrintRoute();
        friend std::ostream& operator<<(std::ostream& os, const route& r);

        //getters
        std::vector<std::string>                        getAllowedMethods() const;
        std::string                                     getRoot() const;
        std::string                                     getAlias() const;

        std::pair<std::size_t, std::string>             getRedirect() const;
        bool                                            getAutoIndex() const;
        std::string                                     getIndex() const;
        std::string                                     getActualPath() const;
        // std::string                                     getFileUpload() const;
        std::unordered_map<std::string, std::string>    getCgi() const;
        bool                                            getRouteIsRedirect() const;

        // setters
        void    setAllowedMethods(const std::string& option);
        void    setRoot(const std::string& path);
        void    setAlias(const std::string& alias);
        void    setActualPath(const std::string& path);

        void    setRedirect(std::size_t errorNbr, const std::string &path);
        void    setAutoIndex(bool option);
        void    setIndex(const std::string& index);
        // void    setFileUpload(const std::string& path);
        void    setCgi(const std::string& type, const std::string& path);
        void    setRouteIsRedirect(bool option);
};
