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
// #include "Connection.hpp"

#ifndef CGI_TIMEOUT
#define CGI_TIMEOUT 3000
#endif

class Connection;

class ChildError: public std::exception 
{
};

enum CGI_STATE {
    INIT,
    WAIT,
    WRITE,
    READ,
    FINISH,
    ERROR
};

class CGI {
private:
    CGI_STATE                   _state;
    std::vector<char *>         _envp;
    std::vector<char *>         _argv;

    int                         _pid;
    int                         _pipeIn[2];
    int                         _pipeOut[2];
    int                         _wpidstatus;
#ifndef __APPLE__
    std::chrono::time_point<std::chrono::system_clock> _start;
#else
	std::chrono::time_point<std::chrono::steady_clock> _start;
#endif //!__APPLE__
    std::string                 _output;

    std::string                 _method;
    std::string                 _script;
    std::string                 _content_type;
    std::string                 _content_length;
    std::string                 _query_string;
    std::string                 _path_info;
    std::string                 _cgi_engine;
    std::string                 _script_name;
    std::string                 _body;


public:

    size_t                     _write_progress;
    std::vector<std::string>    _env;


    CGI();
    // CGI(const Request& request);
    ~CGI();

    CGI_STATE    getCgiState() const;
    std::string  getMethod() const;
    std::string  getScript() const;
    std::string  getContentType() const;
    std::string  getContentLength() const;
    std::string  getQueryString() const;
    std::string  getPathInfo() const;
    std::string  getCgiEngine() const;
    std::string  getOutput() const;

    void    setCgiState(const CGI_STATE& value);
    void    setMethod(const std::string& method);
    void    setScript(const std::string& script);
    void    setContentType(const std::string& type);
    void    setContentLength(const std::string& length);
    void    setQueryString(const std::string& query);
    void    setPathInfo(const std::string& path);
    void    setCgiEngine(const std::string& engine);
    void    setEnvp(std::string& env);
    void    setBody(const std::string& str);
    void    setOutput(const std::string& str);




    // void    runCgi();
    void    cgiProcess();
    void    readCgiOutput(Connection& con);
    void    setup_connection(Connection& con);
    void    writing(Connection& con);
    void    waiting(Connection& con);
    void    pipe_cleaner(Connection& con);

    std::string parseCgiOutput();
};
