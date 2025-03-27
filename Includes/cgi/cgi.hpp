#pragma once

#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <chrono>
//#include "request.hpp"
#include <algorithm>
#include <cctype>
#include <signal.h>
#include <iostream>
#include <filesystem>

#ifndef CGI_TIMEOUT
#define CGI_TIMEOUT 3000
#endif

enum CGI_STATE {
    INIT,
    WAIT,
    WRITE,
    READ,
    FINISH
};

class CGI {
private:
    bool                        _is_finished;
    CGI_STATE                   _state;
    // std::vector<std::string>    _env;
    std::vector<char *>         _envp;
    std::vector<char *>         _argv;

    int                         _pid;
    int                         _pipeIn[2];
    int                         _pipeOut[2];
    int                         _wpidstatus;
    std::chrono::time_point<std::chrono::steady_clock> _start;

    std::string                 _output;

    std::string                 _method;
    std::string                 _script;
    std::string                 _content_type;
    std::string                 _content_length;
    std::string                 _query_string;
    std::string                 _path_info;
    std::string                 _cgi_engine;


public:
    CGI();
    // CGI(const Request& request);
    ~CGI();

    bool        getIsFinished() const;
    CGI_STATE   getCgiState() const;

    void    setIsFinished(bool option);
    void    setCgiState(CGI_STATE value);


    void    runCgi();
    void    cgiProcess();
    void    readCgiOutput();

};