#ifndef HTTPRESPONSEBUILDER_HPP
# define HTTPRESPONSEBUILDER_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "ErrorHandler.hpp"
# include "ServerConfig.hpp"
# include "StaticFileHandler.hpp"

class ResponseBuilder {
private:
    ErrorHandler                error_handler;
    StaticFileHandler           static_handler;

public:
    ResponseBuilder(const ServerConfig& config);
    ~ResponseBuilder();

    std::string     generateDirectoryListing(const std::string& path) const;

    void	    buildResponse(Request& request, Response& response);

    void	    handleRedirect(Request& request, Response& response) const;
    void	    handleAutoIndex(const std::string& path, Response& response) const;
    void	    handleGet(const Request& request, const Location& location, Response& response);
    void	    handleDelete(const std::string& full_path, Response& response);
    void	    handleCgi(Request& request, Response& response);
    void        handlePost(Response& response);
};

#endif
