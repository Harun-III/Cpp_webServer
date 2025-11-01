#include "SessionManager.hpp"

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

std::string SessionManager::generateSessionId() const {
    std::stringstream ss;
    
    // Combine multiple sources of randomness
    ss << std::time(NULL);
    ss << "_";
    ss << std::clock();
    ss << "_";
    ss << std::rand();
    
    return ss.str();
}

std::string SessionManager::createSession() {
    std::string session_id = generateSessionId();
    sessions[session_id] = SessionData();
    return session_id;
}

bool SessionManager::sessionExists(const std::string& session_id) const {
    return sessions.find(session_id) != sessions.end();
}

SessionData& SessionManager::getSession(const std::string& session_id) {
    std::map<std::string, SessionData>::iterator it = sessions.find(session_id);
    if (it != sessions.end()) {
        it->second.last_access = std::time(NULL);
        return it->second;
    }
    
    // If session doesn't exist, create new one
    sessions[session_id] = SessionData();
    return sessions[session_id];
}

void SessionManager::updateSession(const std::string& session_id, const SessionData& data) {
    if (sessionExists(session_id)) {
        sessions[session_id] = data;
        sessions[session_id].last_access = std::time(NULL);
    }
}

void SessionManager::deleteSession(const std::string& session_id) {
    sessions.erase(session_id);
}

void SessionManager::cleanupOldSessions(time_t max_age) {
    time_t now = std::time(NULL);
    std::map<std::string, SessionData>::iterator it = sessions.begin();
    
    while (it != sessions.end()) {
        if (now - it->second.last_access > max_age) {
            sessions.erase(it++);
        } else {
            ++it;
        }
    }
}
