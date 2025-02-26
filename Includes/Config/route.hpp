#pragma once
#include <string>
#include <vector>
#include <map>

class route {

    private:
        // both
        bool							    _GETOption;
        bool								_POSTOption;
        std::string							_root;
        // route specific
        // std::pair<std::size_t,std::string>	_redirect;
        bool								_autoindex;
        std::vector<std::string>			_index;
        std::string                         _file_upload;

    public:
        route();
        ~route();

        //getters
        bool                                getGETOption() const;
        bool                                getPOSTOption() const;
        std::string                         getRoot() const;

        // std::pair<std::size_t, std::string> getRedirect();
        bool                                getAutoIndex();
        std::vector<std::string>            getIndex();
        std::string                         getFileUpload();

        // setters
        void    setGetOption(bool option);
        void    setPostOption(bool option);
        void    setRoot(const std::string& path);

        // void    setRedirect(std::size_t errorNbr, std::string path);
        void    setAutoIndex(bool option);
        void    setIndex(const std::vector<std::string>& indexes);
        void    setFileUpload(const std::string& path);
};