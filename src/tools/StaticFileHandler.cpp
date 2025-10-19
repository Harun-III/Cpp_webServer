#include <algorithm>
#include <dirent.h>
#include <cstdio>
#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler() {
}

StaticFileHandler::~StaticFileHandler() {
}

std::string StaticFileHandler::getMimeType(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }

    std::string extension = path.substr(dot_pos + 1);

    // Convert to lowercase
    for (std::string::iterator it = extension.begin(); it != extension.end(); ++it) {
        *it = std::tolower(static_cast<unsigned char>(*it));
    }

    if (extension == "txt" || extension == "cpp" || extension == "hpp"
            || extension == "h")  return "text/plain";
    if (extension == "md")   return "text/markdown";
    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css")  return "text/css";
    if (extension == "xml")  return "text/xml";
    if (extension == "csv")  return "text/csv";
    if (extension == "js" || extension == "mjs") return "application/javascript";
    if (extension == "json") return "application/json";
    if (extension == "jsonld") return "application/ld+json";

    if (extension == "png")  return "image/png";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "gif")  return "image/gif";
    if (extension == "bmp")  return "image/bmp";
    if (extension == "ico")  return "image/x-icon";
    if (extension == "svg" || extension == "svgz") return "image/svg+xml";
    if (extension == "webp") return "image/webp";
    if (extension == "tif" || extension == "tiff") return "image/tiff";
    if (extension == "avif") return "image/avif";

    if (extension == "mp3")  return "audio/mpeg";
    if (extension == "wav")  return "audio/wav";
    if (extension == "ogg")  return "audio/ogg";
    if (extension == "m4a")  return "audio/mp4";
    if (extension == "weba") return "audio/webm";

    if (extension == "mp4")  return "video/mp4";
    if (extension == "m4v")  return "video/mp4";
    if (extension == "mov")  return "video/quicktime";
    if (extension == "avi")  return "video/x-msvideo";
    if (extension == "wmv")  return "video/x-ms-wmv";
    if (extension == "ogv")  return "video/ogg";
    if (extension == "webm") return "video/webm";
    if (extension == "3gp")  return "video/3gpp";
    if (extension == "3g2")  return "video/3gpp2";
    if (extension == "mkv")  return "video/x-matroska";

    if (extension == "pdf")  return "application/pdf";
    if (extension == "ps")   return "application/postscript";
    if (extension == "eps")  return "application/postscript";
    if (extension == "ai")   return "application/postscript";
    if (extension == "rtf")  return "application/rtf";
    if (extension == "doc")  return "application/msword";
    if (extension == "docx") return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (extension == "xls")  return "application/vnd.ms-excel";
    if (extension == "xlsx") return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if (extension == "ppt")  return "application/vnd.ms-powerpoint";
    if (extension == "pptx") return "application/vnd.openxmlformats-officedocument.presentationml.presentation";

    if (extension == "zip")  return "application/zip";
    if (extension == "gz")   return "application/gzip";
    if (extension == "tar")  return "application/x-tar";
    if (extension == "bz2")  return "application/x-bzip2";
    if (extension == "xz")   return "application/x-xz";
    if (extension == "7z")   return "application/x-7z-compressed";
    if (extension == "rar")  return "application/vnd.rar";

    if (extension == "wasm") return "application/wasm";
    if (extension == "sh")   return "application/x-sh";
    if (extension == "csh")  return "application/x-csh";
    if (extension == "php")  return "application/x-httpd-php";
    if (extension == "swf")  return "application/x-shockwave-flash";

    if (extension == "otf")  return "font/otf";
    if (extension == "ttf")  return "font/ttf";
    if (extension == "woff") return "font/woff";
    if (extension == "woff2") return "font/woff2";

    if (extension == "mpkg") return "application/vnd.apple.installer+xml";
    if (extension == "deb")  return "application/vnd.debian.binary-package";
    if (extension == "iso")  return "application/x-iso9660-image";
    if (extension == "dmg")  return "application/x-apple-diskimage";

    return "application/octet-stream";
}

bool StaticFileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool StaticFileHandler::isDirectory(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

bool StaticFileHandler::isReadable(const std::string& path) const {
    return (access(path.c_str(), R_OK) == 0);
}

std::string StaticFileHandler::readFile(const std::string& path) const {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

bool    StaticFileHandler::deleteFile(const std::string& path) const {
    return (std::remove(path.c_str()) == 0);
}

std::vector<std::string>    StaticFileHandler::listDirectory(const std::string& path) const {
    std::vector<std::string> entries;

    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        return entries;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // Skip . and ..
        if (name == "." || name == "..") {
            continue;
        }

        // Add "/" to directories
        std::string full_path = path + "/" + name;
        if (isDirectory(full_path)) {
            name += "/";
        }

        entries.push_back(name);
    }

    closedir(dir);

    std::sort(entries.begin(), entries.end());

    return entries;
}

std::string StaticFileHandler::getContentType(const std::string& path) const {
    return getMimeType(path);
}

bool StaticFileHandler::isPathSafe(const std::string& path) const {
    // Check for directory traversal attempts
    if (path.find("..") != std::string::npos) {
        return false;
    }

    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        return false;
    }

    return true;
}
