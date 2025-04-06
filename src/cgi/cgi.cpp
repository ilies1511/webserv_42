#include "../../Includes/cgi/cgi.hpp"
#include <vector>
#include <filesystem>
#include <printer.hpp>
#include <Server.hpp>
#include "Connection.hpp"
// CGI::CGI() :    _is_finished(false),
//                 _state(INIT) {
//     _method = "GET";
//     _query_string = "name=joe&name=bill";
//     _script = "/Users/sparth/Documents/Tests/webservTests/get.py";
//     // _script = "/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py";
//
//     // _env.emplace_back("REQUEST_METHOD=GET");
//     // _env.emplace_back("SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/hello.py");
//     // _env.emplace_back("QUERY_STRING=name=joe&name=bill");
//     // _env.emplace_back("CONTENT_TYPE=");
//     // _env.emplace_back( "CONTENT_LENGTH=");
//     // _env.emplace_back("PATH_INFO=");
//     // todo add header !!!!
//     _envp.emplace_back((char *)"REQUEST_METHOD=GET");
//     _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/get.py");
//     // _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/getWait.py"); // Test Timeout
//     // _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py");
//     _envp.emplace_back((char *)"QUERY_STRING=name=joe&name=bill");
//     _envp.emplace_back((char *)"CONTENT_TYPE=");
//     _envp.emplace_back((char *) "CONTENT_LENGTH=");
//     _envp.emplace_back((char *)"PATH_INFO=");
//     _envp.emplace_back(nullptr);
//
//     _argv.emplace_back((char *)"/usr/local/bin/python3.9");
//     _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/get.py");
//     // _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/getWait.py"); // Test Timeout
//     // _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py");
//     _argv.emplace_back(nullptr);
//
//     // _method = "GET";
//     // _query_string = "name=joe&name=bill";
//     // _script = "/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py";
// }

void    CGI::pipe_cleaner(Connection& con)
{
    if (_pipeIn[1] != -1) {
        con._server.ft_closeNclean(_pipeIn[1]);
        _pipeIn[1] = -1;
    }
    if (_pipeOut[0] != -1) {
        con._server.ft_closeNclean(_pipeOut[0]);
        _pipeOut[0] = -1;
    }
}


CGI::CGI() :    _state(INIT),
                _wpidstatus(0),
                _write_progress(0)
{
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
}

CGI::~CGI() {
    if (_pid > 0) {
        kill(_pid, SIGKILL);
    }
    if (_pipeIn[1] != -1) {
        ::close(_pipeIn[1]);
    }
    if (_pipeIn[0] != -1) {
        ::close(_pipeIn[0]);
    }
    if (_pipeOut[0] != -1) {
        ::close(_pipeOut[0]);
    }
    if (_pipeOut[1] != -1) {
        ::close(_pipeOut[1]);
    }
}

std::string toUpperCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// CGI::CGI(const Request& request) :  _is_finished(false),
//                                     _state(INIT) {
//     if (request.method.has_value()) {
//         _method = request.method.value();
//         _env.emplace_back("REQUEST_METHOD=" + toUpperCase(_method) );
//     }
//     if (_method == "POST" && request.body.has_value()) {
//         _env.emplace_back("CONTENT_LENGTH=" + std::to_string(request.body.value().size()));
//     }
//     if (request.uri.has_value()) {
//         if (!request.uri->query.empty()) {
//             _env.emplace_back("QUERY_STRING=" + request.uri->query);
//         }
//     }
//     if (_method == "POST") {
//         std::string temp = request.headers.find("Content-Length");
//     }
// }

CGI_STATE CGI::getCgiState() const {
    return _state;
}

std::string CGI::getMethod() const {
    return _method;
}

std::string CGI::getScript() const {
    return _script;
}

std::string CGI::getContentType() const {
    return _content_type;
}

std::string CGI::getContentLength() const {
    return _content_length;
}

std::string CGI::getQueryString() const {
    return _query_string;
}

std::string CGI::getPathInfo() const {
    return _path_info;
}

std::string CGI::getCgiEngine() const {
    return _cgi_engine;
}


std::string CGI::getOutput() const {
    return _output;
}

void    CGI::setMethod(const std::string& method) {
    _method = method;
}

void    CGI::setScript(const std::string& script) {
    _script = script;
}

void    CGI::setContentType(const std::string &type) {
    _content_type = type;
}

void    CGI::setContentLength(const std::string &length) {
    _content_length = length;
}

void    CGI::setQueryString(const std::string &query) {
    _query_string = query;
}

void    CGI::setPathInfo(const std::string &path) {
    _path_info = path;
}

void    CGI::setCgiState(const CGI_STATE &value) {
   _state = value;
}

void    CGI::setCgiEngine(const std::string &engine) {
    _cgi_engine = engine;
}

void CGI::setEnvp(std::string& env) {
    _envp.emplace_back(env.data());
}

void CGI::setBody(const std::string &str) {
    _body = str;
}

void CGI::setOutput(const std::string &str) {
    _output = str;
}

std::string CGI::parseCgiOutput() {
    std::string mod;
    if (_output.compare(0, 8, "Status: ") == 0) {
        return "HTTP/1.1 " + _output.substr(8);
    }
    return "HTTP/1.1 200 OK\r\n" + _output;
}



void CGI::cgiProcess() {
    // std::cerr << "CHECK FROM CGI PROCESS" << std::endl;
    // std::cerr << _argv[0] << std::endl;
    // std::cerr << _argv[1] << std::endl;
    if (close(_pipeIn[1]) == -1 || close(_pipeOut[0]) == -1
    || dup2(_pipeIn[0], STDIN_FILENO) == - 1 || dup2(_pipeOut[1], STDOUT_FILENO) == -1
    || close(_pipeIn[0]) == -1 || close(_pipeOut[1]) == -1) {
        std::cerr << "Internal CGI error" << std::endl;
        throw ChildError();
    }

    std::size_t pos = _script.rfind('/');
    if (pos != std::string::npos) {
        std::string temp = _script.substr(0, pos);
        std::filesystem::current_path(_script.substr(0, pos));
    }
    #ifdef DEBUG
    std::cerr << "script location: " << std::filesystem::current_path() << std::endl;
    #endif
    execve(_argv[0], _argv.data(), _envp.data());
    std::cerr << "CGI execution failed" << std::endl;
    close(_pipeIn[0]);
    close(_pipeOut[1]);
    throw ChildError();
}

void CGI::readCgiOutput(Connection& con) {
    constexpr size_t BUFFER_S = 52428800; // 50 MB
    // constexpr size_t BUFFER_S = 5242880; // 5 MB
    // constexpr size_t BUFFER_S = 96; // 50 MB
    // std::vector<char> buffer(BUFFER_S);
    // std::vector<char> buffer;
    static char buffer[BUFFER_S];
    ssize_t bytesRead = 0;

    // if (con.check_revent(_pipeOut[0], POLLHUP)) {
    //     // con._server.ft_closeNclean(_pipeOut[0]);
    //     // _pipeOut[0] = -1;
    //     pipe_cleaner(con);
    //     _state = ERROR;
    //     return ;
    // }
    if (!con.check_revent(_pipeOut[0], POLLIN)) {
        std::cout << "!poll in" << std::endl;
        return ;
    }
        std::cout << "poll in" << std::endl;
    bytesRead = read(_pipeOut[0], buffer, BUFFER_S);
    if (bytesRead < 0) {
        std::cerr << "Error reading from pipe." << std::endl;
        // close(_pipeOut[0]);
        // con._server.ft_closeNclean(_pipeOut[0]);
        // _pipeOut[0] = -1;
        pipe_cleaner(con);
        _state = ERROR;
        // _is_finished = true;
    }
    if (bytesRead == 0) {
        std::cout << "Finish reading" << std::endl;
        // con._server.ft_closeNclean(_pipeOut[0]);
        // // close(_pipeOut[0]);
        // _pipeOut[0] = -1;
        pipe_cleaner(con);
        _state = FINISH;
        // _is_finished = true;
        // std::cout << "read Data: " << _output << std::endl;
    }
    _output.append(buffer, static_cast<size_t>(bytesRead));
}

void    CGI::setup_connection(Connection& con) {
    _envp.emplace_back(nullptr);
    _argv.emplace_back(_cgi_engine.data());
    size_t pos = _script.rfind('/');
    if (pos != std::string::npos) {
        _script_name = _script.substr(pos + 1, _script.length());
    } else {
        _script_name = _script;
    }
    _argv.emplace_back(_script_name.data());
    _argv.emplace_back(nullptr);
    // for (auto word : _envp) {
    //     std::cout << coloring(word, BLUE) << std::endl;
    // }
    if (pipe(_pipeIn) == -1) {
        std::cout << "pipe failed" << std::endl;
        _state = ERROR;
        return;
    }
    if (pipe(_pipeOut) == -1) {
        close(_pipeIn[0]);
        close(_pipeIn[1]);
        _pipeIn[0] = -1;
        _pipeIn[1] = -1;
        std::cout << "pipe failed" << std::endl;
        _state = ERROR;
        return;
    }
    // fcntl(_pipeIn[1], F_SETFD, O_NONBLOCK); // write end
    // fcntl(_pipeOut[0], F_SETFD, O_NONBLOCK); // read end

    _pid = fork();
    if (_pid == -1) {
        std::cout << "fork failed" << std::endl;
        close(_pipeIn[0]);
        close(_pipeIn[1]);
        close(_pipeOut[0]);
        close(_pipeOut[1]);
        _pipeIn[0] = -1;
        _pipeIn[1] = -1;
        _pipeOut[0] = -1;
        _pipeOut[1] = -1;
        _state = ERROR;
        return;
    }
    if (_pid == 0) {
        cgiProcess();
    }
    if (close(_pipeIn[0]) == -1) {
        std::cout << "closing pipe failed" << std::endl;
        close(_pipeIn[1]);
        close(_pipeOut[0]);
        close(_pipeOut[1]);
        _pipeIn[0] = -1;
        _pipeIn[1] = -1;
        _pipeOut[0] = -1;
        _pipeOut[1] = -1;
        _state = ERROR;
        return;
    }
    if (close(_pipeOut[1])) {
        std::cout << "closing pipe failed" << std::endl;
        close(_pipeIn[1]);
        close(_pipeOut[0]);
        _pipeIn[0] = -1;
        _pipeIn[1] = -1;
        _pipeOut[0] = -1;
        _pipeOut[1] = -1;
        _state = ERROR;
        return;
    }
    con._server.add_to_pollfds(_pipeIn[1]);
    con._server.add_to_pollfds(_pipeOut[0]);
    con._server.setup_non_blocking(_pipeIn[1]);
    con._server.setup_non_blocking(_pipeOut[0]);

    if (_method == "POST") {
        _state = WRITE;
    } else {
        _state = WAIT;
        con._server.ft_closeNclean(_pipeIn[1]);
        _start = std::chrono::high_resolution_clock::now();
    }
}

// #include <poll.h>
#include <cassert>
// TODO: check useful events for pipefds
void    CGI::writing(Connection& con) {
    if (con.check_revent(_pipeIn[1], POLLNVAL)) {
        std::cout << "wpoll perr" << std::endl;
        pipe_cleaner(con);
        _state = ERROR;
        return ;
    }
    if (con.check_revent(_pipeIn[1], POLLERR)) {
        std::cout << "wpoll perr" << std::endl;
        pipe_cleaner(con);
        _state = ERROR;
        return ;
    }
    if (con.check_revent(_pipeIn[1], POLLHUP)) {
        std::cout << "wpoll phup" << std::endl;
        pipe_cleaner(con);
        _state = ERROR;
        return ;
    } else if (!con.check_revent(_pipeIn[1], POLLOUT)) {
        std::cout << "!wpoll pout" << std::endl;
        // con.ft_closeNcleanRoot(_pipeIn[1]);
        // pipe_cleaner(con);
        return ;
    }
        std::cout << "wpoll pout" << std::endl;
    // Todo Should be non-blocking !!!
    //write(_pipeIn[1], _body.data(), _body.length());
	//05.04 fabi: replaced debugging write size I think? (to be able to upload large files fast)
    //ssize_t written = write(_pipeIn[1], _body.data() + _write_progress, 7 < _body.length() - _write_progress ? 7 : _body.length() - _write_progress);
    ssize_t written = write(_pipeIn[1], _body.data() + _write_progress, _body.length() - _write_progress);
    if (written == -1) {
        pipe_cleaner(con);
        _state = ERROR;
        return;
    } else {
        _write_progress += static_cast<size_t>(written);
    }

    if (_body.length() == _write_progress || written == 0) {
        con._server.ft_closeNclean(_pipeIn[1]);
        _state = WAIT;
        _start = std::chrono::high_resolution_clock::now();
    } //else if() {
       // std::cout << coloring("TO MUCH WRITTEN", BLUE) << std::endl;
    //}

    // assert(_body.length() >= _write_progress && "TO MUCH WRITTEN");
}

void    CGI::waiting(Connection& con) {
    (void)con;
    // auto start = std::chrono::high_resolution_clock::now();
    pid_t process = waitpid(_pid, &_wpidstatus, WNOHANG);

    auto current = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - _start);
    // std::cout << "duration: " << duration.count() << std::endl;
    if (duration.count() >= CGI_TIMEOUT) {
        std::cout << "CGI_TIMEOUT after " << duration.count() << std::endl;
        kill(_pid, SIGKILL);
        pipe_cleaner(con);
        _pid  = -1;
        _state = ERROR;
        // setCgiState(FINISH);
        // _is_finished = true;
    } else if (process == _pid) {
        _pid  = -1;
        std::cout << "Script finished in " << duration.count() << std::endl;
        _state = READ;
    }
    else if (_wpidstatus != 0) {
        std::cout << coloring("Script execution failed", BLUE) << std::endl;
        std::cout << coloring("wpidstatus: " + std::to_string(_wpidstatus), BLUE) << std::endl;
        pipe_cleaner(con);
        _state = ERROR;
    }
}
