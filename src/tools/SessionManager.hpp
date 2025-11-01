#ifndef SESSIONMANAGER_HPP
# define SESSIONMANAGER_HPP

# include "Core.hpp"

struct SessionData {
    std::string theme;      // "light" or "dark"
    std::string language;   // "en" or "fr"
    time_t      last_access;
    
    SessionData() : theme("light"), language("en"), last_access(std::time(NULL)) {}
};

class SessionManager {
private:
    std::map<std::string, SessionData>  sessions;
    
    SessionManager() {}
    SessionManager(const SessionManager&);
    SessionManager& operator=(const SessionManager&);
    
    std::string generateSessionId() const;

public:
    static SessionManager& getInstance();
    
    std::string     createSession();
    bool            sessionExists(const std::string& session_id) const;
    SessionData&    getSession(const std::string& session_id);
    void            updateSession(const std::string& session_id, const SessionData& data);
    void            deleteSession(const std::string& session_id);
    
    void            cleanupOldSessions(time_t max_age = 3600); // 1 hour
};

#endif
