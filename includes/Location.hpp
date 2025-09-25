#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <string>
#include <map>

class Location {
private:
    std::vector<std::string>                methods;
    std::string                             root;
    std::pair<int, std::string>             return_directive; // status code, redirect URL
    std::string                             index;
    bool                                    auto_index;
    bool                                    upload;
    std::string                             upload_location;
    std::map<std::string, std::string>      cgi; // extension -> cgi path

public:
    Location();
    ~Location();

    // Setters
    void                                setMethods(const std::vector<std::string>& methods);
    void                                setRoot(const std::string& root);
    void                                setUploadLocation(const std::string& location);
    void                                setAutoIndex(bool auto_index);
    void                                setUpload(bool upload);
    void                                setIndex(const std::string& index);
    void                                setReturn(int status, const std::string& url);

    // Getters
    const std::string&                  getRoot() const;
    const std::string&                  getUploadLocation() const;
    bool                                getAutoIndex() const;
    bool                                getUpload() const;
    const std::string&                  getIndex() const;
    const std::pair<int, std::string>&  getReturn() const;



};

#endif

