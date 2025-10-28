# include "Core.hpp"
# include "CgiHandler.hpp"

CgiHandler::CgiHandler(const Request& req, const Location& loc)
    : request(req), location(loc), script_path(""), cgi_executable(""),
    start_time(0), pid(-1), cgi_status(CGI_INIT), env(NULL), args(NULL),
    output_fd(-1), cgi_output("") {
}

CgiHandler::~CgiHandler() {
    // Clean up if destructor is called before completion
    if (output_fd != -1) {
        close(output_fd);
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

std::string CgiHandler::generateOutputFilename() const {
	std::time_t wallTime = std::time(NULL);
	std::clock_t cpuTicks = std::clock();
	std::ostringstream orand;
	orand << "/tmp/cgi_output_" << wallTime << "_" << cpuTicks << "_" << pid;
	return orand.str();
}

void CgiHandler::execute(Response& response) {
    // ==================== CGI_INIT ====================
    if (cgi_status == CGI_INIT) {
        script_path = request.path;
        cgi_executable = getCgiExecutable(script_path);
        if (cgi_executable.empty()) {
            response.generateErrorPage(request.server, 500);
            return;
        }
        // Check if script exists
        if (access(script_path.c_str(), F_OK) != 0) {
            response.generateErrorPage(request.server, 404);
            return;
        }
        // Check if script is executable
        if (access(script_path.c_str(), X_OK) != 0) {
            response.generateErrorPage(request.server, 403);
            return;
        }
        // Build environment variables and arguments
        env = buildEnvVariables();
        args = buildArguments();
        
        // Generate output filename BEFORE forking so both parent and child have it
        output_file = generateOutputFilename();
        
        // Fork and execute CGI
        pid = fork();
        if (pid < 0) {
            // Fork failed
            response.generateErrorPage(request.server, 500);
            return;
        }
        if (pid == 0) {
            // ==================== CHILD PROCESS ====================
            // Handle input for POST requests
            if (request.method == "POST") {
                if (dup2(request.cgiFd, STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
                close(request.cgiFd);
            }
            // Create output file using the member variable
            int out_fd = open(output_file.c_str(), 
                              O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1) {
                perror("open output file");
                exit(EXIT_FAILURE);
            }
            if (dup2(out_fd, STDOUT_FILENO) == -1) {
                perror("dup2 stdout");
                close(out_fd);
                exit(EXIT_FAILURE);
            }
            close(out_fd);
            // Change to script directory
            std::string script_dir = script_path.substr(0, script_path.find_last_of('/'));
            if (!script_dir.empty()) {
                if (chdir(script_dir.c_str()) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }
            // Get script filename only (no path)
            std::string script_filename = script_path.substr(script_path.find_last_of('/') + 1);
            // Build final arguments: [interpreter, script_name, NULL]
            char* exec_args[3];
            exec_args[0] = args[0];
            exec_args[1] = const_cast<char*>(script_filename.c_str());
            exec_args[2] = NULL;
            // Execute the CGI script
            execve(cgi_executable.c_str(), exec_args, env);
            perror("execve");
            exit(EXIT_FAILURE);
        }
        // ==================== PARENT PROCESS ====================
        // Close the input fd if it was for POST
        if (request.method == "POST" && request.cgiFd != -1) {
            close(request.cgiFd);
        }
        // Record start time and transition to PROCESS state
        start_time = std::time(NULL);
        cgi_status = CGI_PROSSESS;
        return;  // Exit and wait for next call
    }
    // ==================== CGI_PROSSESS ====================
    if (cgi_status == CGI_PROSSESS) {
        int status;

        time_t curr_time = std::time(NULL);
        // Check for timeout
        if (curr_time - start_time >= CGI_TIMEOUT) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            response.generateErrorPage(request.server, 500);
            return;
        }
        // Non-blocking check if child has terminated
        int result = waitpid(pid, &status, WNOHANG);
        if (result == 0) {
            // Child still running, return and check again later
            throw (0, READY_TO_WRITE);
        }
        if (result < 0) {
            // waitpid error
            response.generateErrorPage(request.server, 500);
            return;
        }
        // Child has terminated (result > 0)
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            response.generateErrorPage(request.server, 500);
            return;
        }

        // Open and read the entire file
        file(output_file.c_str(), std::ios::binary);
        if (!file.is_open()) {
            throw Stat(500, BAD);
        }

        // Child exited successfully, transition to END state
        cgi_status = CGI_END;
    }
    // ==================== CGI_END ====================
    if (cgi_status == CGI_END) {
        // Read CGI output
        char buffer[CGI_BUFFER];
        size_t bytes_read;

        file.read(buffer, CGI_BUFFER);
        // gcout() return the number of bytes read
        bytes_read = static_cast<size_t>(file.gcout());
        
        // response.cgi_offset += bytes_read;

        // check for "\r\n\r\n" 
        std::string delimiter = "\r\n\r\n";
        size_t header_end = cgi_output.find(delimiter);
        if (header_end == std::string::npos) {
            delimiter = "\n\n";
            header_end = cgi_output.find(delimiter);
            if (header_end == std::string::npos) {
                response.cgi_offset = header_end;
                // Set default status if not set by CGI
                response.setStatusCode(200);
                parseHeaders(cgi_output, response);
                response.setContentLength(getCgiFileLength(output_file));
                response.generateHead();
                throw (0, WRITING);
            }
        }

        if (cgi_output.size() == 0) {
            throw Stat(500, BAD);
        }

        if (cgi_output.size() > CGI_STRING_MAX_OUTPUT) {
            throw Stat(500, BAD);
        }

        // throw READY_TO_WRITE to stop here and re-enter the loop
        throw (0, READY_TO_WRITE);
    }
}

size_t    CgiHandler::getCgiFileLength(const std::string pathToCgiFile, size_t headSize) const {
    struct stat s_buffer;

    if (stat(pathToCgiFile.c_str(), &s_buffer) != 0) {
        return (0);
    }

    return (s_buffer.st_size - headSize);
}

void CgiHandler::parseHeaders(std::string& cgi_output, Response& response) const {
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
