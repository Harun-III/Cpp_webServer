# include "Core.hpp"
# include "CgiHandler.hpp"
# include "Response.hpp"

CgiHandler::CgiHandler() : script_path(""), cgi_executable(""),
    start_time(0), pid(-1), cgi_status(CGI_INIT), env(NULL), args(NULL),
    cgi_output("") {
}

CgiHandler::~CgiHandler() {
    if (args) {
        freeEnvArray(args);
    }
    if (env) {
        freeEnvArray(env);
    }
}

cgi_e CgiHandler::getStatus() const {
    return cgi_status;
}

void CgiHandler::setStatus(cgi_e status) {
    cgi_status = status;
}

std::string CgiHandler::getFileExtension(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "";
    }
    return path.substr(dot_pos);
}

std::string CgiHandler::getCgiExecutable(Location& location, const std::string& file_path) const {
    std::string extension = getFileExtension(file_path);
    const std::map<std::string, std::string>& cgi_map = location.getCgi();
    std::map<std::string, std::string>::const_iterator it = cgi_map.find(extension);
    if (it != cgi_map.end()) {
        return it->second;
    }
    return "";
}

char** CgiHandler::buildArguments() const {
    char** args = new char*[3];
    
    args[0] = new char[cgi_executable.length() + 1];
    std::strcpy(args[0], cgi_executable.c_str());
    args[1] = NULL;
    args[2] = NULL;
    return args;
}

void    CgiHandler::manageCgifds(const char *file, int oflags, int to_fd) {
    int fd = open(file, oflags, 0644);

    if (fd == -1) {
        perror("open file for cgi");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, to_fd) == -1) {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void CgiHandler::freeEnvArray(char** env) const {
    if (!env) return;
    for (int i = 0; env[i] != NULL; ++i) {
        delete[] env[i];
    }
    delete[] env;
}

std::string CgiHandler::generateOutputFilename() const {
	std::time_t wallTime = std::time(NULL);
	std::clock_t cpuTicks = std::clock();
	std::ostringstream orand;
	orand << "/tmp/server_cgi_tmp_" << wallTime << "_" << cpuTicks;
	return orand.str();
}

void CgiHandler::parseHeaders(std::string& cgi_output, Response& response) const {
    std::stringstream	headersBlock;

    headersBlock << cgi_output;

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

char** CgiHandler::buildEnvVariables (Request& request) const {
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

    char** env = new char*[env_strings.size() + 1];
    for (size_t i = 0; i < env_strings.size(); ++i) {
        env[i] = new char[env_strings[i].length() + 1];
        std::strcpy(env[i], env_strings[i].c_str());
    }
    env[env_strings.size()] = NULL;
    
    return env;
}

size_t    CgiHandler::getCgiFileLength(const std::string pathToCgiFile, size_t headSize) const {
    struct stat s_buffer;

    if (stat(pathToCgiFile.c_str(), &s_buffer) != 0) {
        return (0);
    }

    return (s_buffer.st_size - headSize);
}

void    CgiHandler::checkProcess(Response &response) {
    int         status;
    time_t      curr_time = std::time(NULL);

    // Check for timeout
    if (curr_time - start_time >= CGI_TIMEOUT) {
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);
        throw State(500, BAD);
    }

    // Non-blocking check if child has terminated
    int result = waitpid(pid, &status, WNOHANG);

    // Child still running
    if (result == 0) {
        throw State(0, READY_TO_WRITE);
    }

    // Error occurred
    if (result < 0) {
        throw State(500, BAD);
    }

    // Check exit status
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        throw State(500, BAD);
    }

    // Open the CGI output file
    response.bodyStream.open(output_file.c_str(), std::ios::binary);

    if (!response.bodyStream.is_open()) {
        throw State(500, BAD);
    }
    
    setStatus(CGI_END);
    response.cgiFilePath = output_file;
    throw State(0, READY_TO_WRITE);
}

void CgiHandler::processOutput(Response& response) {
    char *buffer = new char[CGI_BUFFER];
    size_t bytes_read;

    // Read from bodyStream into buffer
    response.bodyStream.read(buffer, CGI_BUFFER);
    bytes_read = static_cast<size_t>(response.bodyStream.gcount());
    cgi_output.append(buffer, bytes_read);

    delete[] buffer;

    // Check for header delimiter
    std::string delimiter = "\r\n\r\n";
    size_t header_end = cgi_output.find(delimiter);

    if (header_end == std::string::npos) {
        delimiter = "\n\n";
        header_end = cgi_output.find(delimiter);
    }

    // If we FOUND the delimiter, process headers
    if (header_end != std::string::npos) {
        // Extract headers from cgi_output
        std::string headers = cgi_output.substr(0, header_end);
        header_end += delimiter.length();

        if (headers.find(":") == std::string::npos) {
            throw State(500, BAD);
        }

        response.setStatusCode(200);  // Default status
        parseHeaders(headers, response);

        // Seek bodyStream to position after headers (start of body content)
        response.bodyStream.clear(); // Clear any EOF or error flags
        response.bodyStream.seekg(header_end, std::ios::beg);

        size_t con_len = getCgiFileLength(output_file, header_end);
        response.setContentLength(con_len);
        response.generateHead();
        throw State(0, WRITING);
    }

    // Check if we've reached EOF without finding headers
    if (response.bodyStream.eof()) {
        throw State(500, BAD);
    }

    if (cgi_output.size() == 0) {
        throw State(500, BAD);
    }

    if (cgi_output.size() > CGI_STRING_MAX_OUTPUT) {
        throw State(500, BAD);
    }

    // If headers not complete yet, wait for more data
    throw State(0, READY_TO_WRITE);
}

void CgiHandler::execute(Request& request) {
    script_path = request.path;
    cgi_executable = getCgiExecutable(request.location, script_path);

    if (cgi_executable.empty()) {
        throw State(500, BAD);
    }
    // Check if script exists
    if (access(script_path.c_str(), F_OK) != 0) {
        throw State(404, BAD);
    }
    // Check if script is executable
    if (access(script_path.c_str(), X_OK) != 0) {
        throw State(403, BAD);
    }
    // Build environment variables and arguments
    env = buildEnvVariables(request);
    args = buildArguments();
    
    // Generate output filename BEFORE forking so both parent and child have it
    output_file = generateOutputFilename();

    // Fork and execute CGI
    pid = fork();
    if (pid < 0) {
        // Fork failed
        throw State(500, BAD);
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

        manageCgifds("./.objects/error_cgi.log", O_WRONLY | O_CREAT | O_APPEND, STDERR_FILENO);
        manageCgifds(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, STDOUT_FILENO);

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

    setStatus(CGI_PROSSESS);
    throw State(0, READY_TO_WRITE);
}
