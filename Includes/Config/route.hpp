#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>

class route {

    private:
        // both
        std::vector<std::string>                        _limit_except;
        // bool							    _GETOption;
        // bool								_POSTOption;
        std::string							            _root;
        // route specific
        std::pair<std::size_t,std::string>	            _redirect;
        // std::string                         _location;
        bool								            _autoindex;
        std::vector<std::string>			            _index;
        // std::string                                     _file_upload;
        std::unordered_map<std::string, std::string>    _cgi;

    public:
        route();
        ~route();

        void PrintRoute();
        friend std::ostream& operator<<(std::ostream& os, const route& r);

        //getters
        std::vector<std::string>                        getLimitsExcept() const;
        // bool                                            getGETOption() const;
        // bool                                            getPOSTOption() const;
        std::string                                     getRoot() const;

        std::pair<std::size_t, std::string>             getRedirect();
        bool                                            getAutoIndex() const;
        std::vector<std::string>                        getIndex() const;
        // std::string                                     getFileUpload() const;
        std::unordered_map<std::string, std::string>    getCgi() const;

        // setters
        void    setLimitsExcept(const std::string& option);
        // void    setGetOption(bool option);
        // void    setPostOption(bool option);
        void    setRoot(const std::string& path);

        void    setRedirect(std::size_t errorNbr, const std::string &path);
        void    setAutoIndex(bool option);
        void    setIndex(const std::string& index);
        // void    setFileUpload(const std::string& path);
        void    setCgi(const std::string& type, const std::string& path);
};
