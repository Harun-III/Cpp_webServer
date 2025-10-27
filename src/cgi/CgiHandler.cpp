# include "Core.hpp"
# include "CgiHandler.hpp"

CgiHandler::CgiHandler(const Request& req, const Location& loc)
    : request(req), location(loc), script_path(""), cgi_executable(""),
    start_time(0), pid(-1), cgi_status(CGI_INIT), env(NULL), args(NULL),
    output_file(""), output_fd(-1) {
}

CgiHandler::~CgiHandler() {
    // Clean up if destructor is called before completion
    if (output_fd != -1) {
        close(output_fd);
    }
    if (!output_file.empty()) {
        std::remove(output_file.c_str());
    }
    if (args) {
        freeEnvArray(args);
    }
    if (env) {
        freeEnvArray(env);
    }
}

std::string CgiHandler::getFileExtension(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "";
    }
    return path.substr(dot_pos);
}

std::string CgiHandler::getCgiExecutable(const std::string& file_path) const{
    std::string extension = getFileExtension(file_path);
    const std::map<std::string, std::string>& cgi_map = location.getCgi();

    std::map<std::string, std::string>::const_iterator it = cgi_map.find(extension);
    if (it != cgi_map.end()) {
        return it->second;
    }
    return "";
}

void CgiHandler::execute(Response& response) {
    // Set script path and CGI executable    
    if (cgi_status == CGI_INIT) {
        script_path = request.path;
        cgi_executable = getCgiExecutable(script_path);
        if (cgi_executable.empty()) {
            response.generateErrorPage(request.server, 500);
            return;
        }
    
        // Check if script exists and is executable
        std::cout << "Script to check: " << script_path.c_str() << std::endl;
        if (access(script_path.c_str(), F_OK) != 0) {
            response.generateErrorPage(request.server, 404);
            return;
        }
        
        if (access(script_path.c_str(), X_OK) != 0) {
            response.generateErrorPage(request.server, 403);
            return;
        }

        // Build environment variables and arguments
        args = buildArguments();
        env = buildEnvVariables();

        // Create output pipe first (always needed)
        if (pipe(pipe_out) < 0) {
            response.generateErrorPage(request.server, 500);
            return;
        }

/////// TEST: print env and args
std::cout << "I'll excute the script: " << args[0] << std::endl;
std::cout << "Using the following env:" << std::endl;
for (int i = 0; env && env[i]; i++) {
    std::cout << env[i] << std::endl;
}
/////// TEST: end

        // Fork and execute CGI
        pid = fork();

        if (pid < 0) {
            // fork failed
            close(pipe_out[0]);
            close(pipe_out[1]);
            freeEnvArray(args);
            freeEnvArray(env);
            response.generateErrorPage(request.server, 500);
            return;
        }

        if (pid == 0) {
            //child
            if (request.method == "POST") {
                dup2(request.cgiFd, STDIN_FILENO);
                close(request.cgiFd);
            }

            dup2(pipe_out[1], STDOUT_FILENO);
            close (pipe_out[0]);
            close (pipe_out[1]);

            std::string script_dir = script_path.substr(0, script_path.find_last_of('/'));
            if (!script_dir.empty()) {
                chdir(script_dir.c_str());
            }

            // python3 ./bin/test
            std::string script_filename = script_path.substr(script_path.find_last_of('/') + 1);

            char* new_args[3];
            new_args[0] = args[0];
            new_args[1] = const_cast<char*>(script_filename.c_str());
            new_args[2] = NULL;

            execve(cgi_executable.c_str(), new_args, env);
            perror("execve");
            exit (EXIT_FAILURE);
        }

        // parent
        start_time = std::time(NULL);
        cgi_status = CGI_PROSSESS;
        close (pipe_out[1]);
    } // END

    if (cgi_status == CGI_PROSSESS) {
        int status;
        time_t curr_time = time(NULL);
        int result = waitpid(pid, &status, WNOHANG);

        if (curr_time - start_time >= CGI_TIMEOUT) {
            kill (pid, SIGKILL);
            result = -1;
        }

        if (!result) return ;

        else if (result < 0 || (result > 0 && (!WIFEXITED(status)
                || WEXITSTATUS(status) != 0))) {
            // Error occurred
            close(pipe_out[0]);
            if (request.method == "POST") {
                // NOTE: should delete the file here
                // std::remove(input_file.c_str());
            }
            freeEnvArray(args);
            freeEnvArray(env);
            response.generateErrorPage(request.server, 500);
            return;
        }
        // Child has terminated
        cgi_status = CGI_END;
    }

    if (cgi_status == CGI_END) {
        // Read CGI output
        std::string cgi_output;
        char buffer[BUFFERREADSIZE];
        ssize_t bytes_read;
        
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            cgi_output.append(buffer, bytes_read);
        }
        
        close (pipe_out[0]);
        
        // clean up
        if (request.method == "POST") {
            // NOTE: should delete the file here
            // std::remove(input_file.c_str());
        }
        freeEnvArray(args);
        freeEnvArray(env);
        
        if (cgi_output.empty()) {
            response.generateErrorPage(request.server, 500);
            return;
        }
        
        // Set default status if not set by CGI
        response.setStatusCode(200);
        
        // Parse headers from CGI output
        parseHeaders(cgi_output, response);
        
        response.writeStringToBuffer(cgi_output);
    }
}

void CgiHandler::parseHeaders( std::string& cgi_output, Response& response) const {
    std::string delimiter = "\r\n\r\n";
    size_t header_end = cgi_output.find(delimiter);
    if (header_end == std::string::npos) {
        delimiter = "\n\n";
        header_end = cgi_output.find(delimiter);
        if (header_end == std::string::npos) {
            return ;
        }
    }

    std::stringstream	headersBlock;

    headersBlock << cgi_output.substr(0, header_end);
    cgi_output.erase(0, header_end + delimiter.size());
    std::string			line;

    while (std::getline(headersBlock, line)) {
        if (line.empty()) continue ;

        std::string::size_type	colum = line.find(':');

        std::string header_name = line.substr(0, colum);
        std::string header_value = line.substr(colum + 1);

        if (header_name == "Status") {
            // Parse status code from "Status: 200 OK" format
            size_t space_pos = header_value.find(' ');
            if (space_pos != std::string::npos) {
                int status_code = std::atoi(header_value.substr(0, space_pos).c_str());
                if (status_code > 0) {
                    response.setStatusCode(status_code);
                }
            }
        } else if (header_name == "Content-Type" || header_name == "Content-type") {
            response.setContentType(header_value);
        } else if (header_name == "Location") {
            response.setLocation(header_value);
                response.setStatusCode(302);
        } else {
            response.setHeader(header_name, header_value);
        }
    }
}

char** CgiHandler::buildEnvVariables () const {
    std::vector<std::string> env_strings;

    env_strings.push_back("REQUEST_METHOD=" + request.method);
    env_strings.push_back("SCRIPT_FILENAME=" + script_path);
    env_strings.push_back("SCRIPT_NAME=" + request.target);
    env_strings.push_back("QUERY_STRING=" + request.query);
    env_strings.push_back("SERVER_PROTOCOL=" + request.version);
    env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");

    const std::pair<std::string, std::string>& listen = request.server.getListen()[0];
    env_strings.push_back("SERVER_NAME=" + listen.first);
    env_strings.push_back("SERVER_PORT=" + listen.second);

    if (request.method == "POST") {
        std::stringstream ss;
        ss << request.content_length;
        env_strings.push_back("CONTENT_LENGTH=" + ss.str());
    }

    // Content-type from headers
    map_s::const_iterator content_type = request.headers.find("content-type");
    if (content_type != request.headers.end()) {
        env_strings.push_back("CONTENT_TYPE=" + content_type->second);
    }

    // HTTP headers - these need to be convert to HTTP_* format
    // 4.1.18.  Protocol-Specific Meta-Variables (RFC 3785)
    for (map_s::const_iterator it = request.headers.begin(); 
    it != request.headers.end(); ++it) {
        std::string header_name = it->first;

        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < header_name.length(); ++i) {
            header_name[i] = std::toupper(static_cast<unsigned char>(header_name[i]));
            if (header_name[i] == '-') {
                header_name[i] = '_';
            }
        }

        env_strings.push_back("HTTP_" + header_name + "=" + it->second);
    }

    // 10.12.6.7

    char** env = new char*[env_strings.size() + 1];
    for (size_t i = 0; i < env_strings.size(); ++i) {
        env[i] = new char[env_strings[i].length() + 1];
        std::strcpy(env[i], env_strings[i].c_str());
    }
    env[env_strings.size()] = NULL;
    
    return env;
}

char** CgiHandler::buildArguments() const {
    char** args = new char*[3];
    
    args[0] = new char[cgi_executable.length() + 1];
    std::strcpy(args[0], cgi_executable.c_str());

    args[1] = NULL;
    args[2] = NULL;
    return args;
}

void CgiHandler::freeEnvArray(char** env) const {
    if (!env) return;
    for (int i = 0; env[i] != NULL; ++i) {
        delete[] env[i];
    }
    delete[] env;
}
