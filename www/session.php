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

function findUser($sessions, $username) {
    foreach ($sessions as $index => $session) {
        if ($session[0] === $username) return array('index' => $index, 'user' => $session);
    }
    return null;
}

$action = $_POST['action'];

if ($action === 'signup') {
    $username = trim($_POST['username']);
    $email = trim($_POST['email']);
    $password = trim($_POST['password']);
    
    if (empty($username) || empty($email) || empty($password)) {
        header('Location: auth.html?error=All_fields_required');
        exit();
    }

    $sessions = readSessions();
    if (findUser($sessions, $username) !== null) {
        header('Location: auth.html?error=Username_already_exists');
        exit();
    }

    $sessions[] = array($username, $email, hash('sha256', $password), '', time());
    writeSessions($sessions);
    header('Location: auth.html?success=Account_created_successfully');
    
} elseif ($action === 'login') {
    $username = trim($_POST['username']);
    $password = trim($_POST['password']);
    
    if (empty($username) || empty($password)) {
        header('Location: auth.html?error=All_fields_required');
        exit();
    }

    $sessions = readSessions();
    $userInfo = findUser($sessions, $username);
    
    if ($userInfo === null || $userInfo['user'][2] !== hash('sha256', $password)) {
        header('Location: auth.html?error=Invalid_username_or_password');
        exit();
    }
    
    $token = bin2hex(random_bytes(32));
    $sessions[$userInfo['index']][3] = $token;
    $sessions[$userInfo['index']][4] = time();
    
    writeSessions($sessions);
    setcookie('session_token', $token, time() + 3600, '/');
    header('Location: profile.php');
    
} else {
    header('Location: auth.html');
}
?>
