<?php
$sessions_file = dirname(__FILE__) . '/sessions.csv';

function readSessions() {
    global $sessions_file;
    $sessions = array();
    if (!file_exists($sessions_file))return$sessions;
    $file = fopen($sessions_file, 'r');
    while (($data = fgetcsv($file)) !== FALSE) {
        if (!empty($data[0]))$sessions[]=$data;
    }
    fclose($file);
    return $sessions;
}

function writeSessions($sessions) {
    global $sessions_file;
    $file = fopen($sessions_file, 'w');
    foreach ($sessions as $session) fputcsv($file, $session);
    fclose($file);
}

function getCookie($name) {
    $cookie_env = getenv('COOKIE');
    if ($cookie_env) {
        parse_str(str_replace('; ', '&', $cookie_env), $cookies);
        return isset($cookies[$name]) ? $cookies[$name] : null;
    }
    return null;
}

$token = getCookie('session_token');

if ($token) {
    $sessions = readSessions();
    foreach ($sessions as $index => $session) {
        if (isset($session[3]) && $session[3] === $token) {
            $sessions[$index][3] = '';
            break;
        }
    }
    writeSessions($sessions);
}

setcookie('session_token', '', time() - 3600, '/');
header('Location: auth.html?success=Logged_out_successfully');
?>
