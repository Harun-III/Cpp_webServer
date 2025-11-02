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

function getCookie($name) {
    $cookie_env = getenv('COOKIE');
    if ($cookie_env) {
        parse_str(str_replace('; ', '&', $cookie_env), $cookies);
        return isset($cookies[$name]) ? $cookies[$name] : null;
    }
    return null;
}

function findUserByToken($sessions, $token) {
    foreach ($sessions as $session) {
        if (isset($session[3]) && $session[3] === $token && !empty($token))return$session;
    }
    return null;
}

$token = getCookie('session_token');
$sessions = readSessions();
$user = findUserByToken($sessions, $token);

if ($user === null) {
    header('Location: auth.html?error=Please_login_first');
    exit();
}

$username = $user[0];
$email = $user[1];
$loginTime = isset($user[4]) ? date('Y-m-d H:i:s', $user[4]) : 'Unknown';
$serverAddr = getenv('SERVER_ADDR') ?: getenv('SERVER_NAME') ?: 'N/A';
$serverPort = getenv('SERVER_PORT') ?: 'N/A';
$connection = $serverAddr . ':' . $serverPort;
?>
<!DOCTYPEhtml>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebServ - Dashboard</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Fira+Code:wght@300;400;500;700&display=swap');
        * { margin: 0; padding: 0; box-sizing: border-box; }
        :root {
            --bg-primary: #0a0a0a; --bg-secondary: #1a1a1a; --bg-tertiary: #0d1117;
            --text-primary: #00ff41; --text-secondary: #66ff66; --text-muted: #888;
            --border-color: rgba(0, 255, 65, 0.3); --shadow-color: rgba(0, 255, 65, 0.3);
            --card-bg: rgba(0, 0, 0, 0.8);
        }
        [data-theme="light"] {
            --bg-primary: #fff; --bg-secondary: #f5f5f5; --bg-tertiary: #e8e8e8;
            --text-primary: #00aa33; --text-secondary: #00cc44; --text-muted: #666;
            --border-color: rgba(0, 170, 51, 0.3); --shadow-color: rgba(0, 170, 51, 0.2);
            --card-bg: rgba(255, 255, 255, 0.95);
        }
        body {
            background: linear-gradient(135deg, var(--bg-primary) 0%, var(--bg-secondary) 25%, var(--bg-tertiary) 75%, var(--bg-primary) 100%);
            font-family: 'Fira Code', monospace; color: var(--text-primary); min-height: 100vh; transition: all 0.3s;
        }
        .matrix-bg {
            position: fixed; top: 0; left: 0; width: 100%; height: 100%; opacity: 0.05; z-index: 1;
            background-image: repeating-linear-gradient(0deg, transparent, transparent 2px, var(--border-color) 2px, var(--border-color) 4px),
                              repeating-linear-gradient(90deg, transparent, transparent 2px, var(--border-color) 2px, var(--border-color) 4px);
        }
        .theme-toggle {
            position: fixed; top: 20px; right: 20px; z-index: 100; background: var(--card-bg);
            border: 2px solid var(--text-primary); border-radius: 50px; padding: 10px 20px;
            cursor: pointer; display: flex; align-items: center; gap: 10px;
            box-shadow: 0 0 20px var(--shadow-color); transition: all 0.3s;
        }
        .theme-toggle:hover { box-shadow: 0 0 30px  var(--shadow-color); transform: translateY(-2px); }
        .container { position: relative; z-index: 2; max-width: 900px; margin: 0 auto; padding: 40px 20px; }
        .header { text-align: center; margin-bottom: 50px; }
        .logo {
            font-size: 3.5rem; font-weight: 700; color: var(--text-primary);
            text-shadow: 0 0 20px var(--text-primary); margin-bottom: 10px;
            animation: glow-pulse 3s ease-in-out infinite alternate;
        }
        @keyframes glow-pulse {
            from { text-shadow: 0 0 20px var(--text-primary), 0 0 40px var(--text-primary); }
            to { text-shadow: 0 0 10px var(--text-primary), 0 0 20px var(--text-primary); }
        }
        .subtitle { font-size: 1.2rem; color: var(--text-secondary); letter-spacing: 2px; }
        .dashboard-card {
            background: var(--card-bg); border: 2px solid var(--text-primary); border-radius: 15px;
            padding: 40px; box-shadow: 0 0 30px var(--shadow-color); margin-bottom: 30px;
        }
        .welcome-message { font-size: 2rem; color: var(--text-primary); margin-bottom: 10px; text-align: center; }
        .user-greeting { font-size: 1.5rem; color: var(--text-secondary); text-align: center; margin-bottom: 40px; }
        .info-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .info-item {
            background: rgba(0, 255, 65, 0.1); border: 1px solid var(--border-color);
            border-radius: 10px; padding: 20px; border-left: 4px solid var(--text-primary);
        }
        .info-label {
            color: var(--text-secondary); font-size: 0.85rem; text-transform: uppercase;
            letter-spacing: 1px; margin-bottom: 8px;
        }
        .info-value { color: var(--text-primary); font-size: 1.2rem; font-weight: 600; word-break: break-all; }
        .session-info {
            background: rgba(0, 255, 65, 0.05); border: 1px solid var(--border-color);
            border-radius: 10px; padding: 20px; margin-bottom: 30px;
        }
        .session-title {
            color: var(--text-secondary); font-size: 1.1rem; text-transform: uppercase;
            letter-spacing: 1px; margin-bottom: 15px; display: flex; align-items: center; gap: 10px;
        }
        .session-status {
            display: inline-block; width: 10px; height: 10px; background: var(--text-primary);
            border-radius: 50%; box-shadow: 0 0 10px var(--text-primary); animation: pulse 2s infinite;
        }
        @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
        .session-detail { color: var(--text-muted); font-size: 0.9rem; margin: 5px 0; }
        .session-detail strong { color: var(--text-primary); }
        .actions { display: flex; gap: 15px; justify-content: center; flex-wrap: wrap; }
        .btn {
            padding: 15px 30px; border: 2px solid var(--text-primary); border-radius: 8px;
            font-family: 'Fira Code', monospace; font-weight: 600; font-size: 1rem;
            cursor: pointer; transition: all 0.3s; text-transform: uppercase; letter-spacing: 1px;
            text-decoration: none; display: inline-block;
        }
        .btn-primary { background: none; color: var(--text-primary); }
        .btn-primary:hover {
            background: var(--text-primary); color: var(--bg-primary);
            box-shadow: 0 0 25px var(--shadow-color); transform: translateY(-2px);
        }
        .btn-danger { background: none; border-color: #ff4136; color: #ff4136; }
        .btn-danger:hover { background: #ff4136; color: #000; box-shadow: 0 0 25px rgba(255, 65, 54, 0.5); transform: translateY(-2px); }
        .stats { text-align: center; color: var(--text-muted); font-size: 0.85rem; margin-top: 30px; }
        @media (max-width: 768px) {
            .dashboard-card { padding: 25px; }
            .welcome-message { font-size: 1.5rem; }
            .user-greeting { font-size: 1.2rem; }
            .info-grid { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="matrix-bg"></div>
    <div class="theme-toggle" onclick="toggleTheme()">
        <span class="theme-icon" id="themeIcon">🌙</span>
        <span style="color: var(--text-primary); font-weight: 500; font-size: 0.9rem;" id="themeText">Dark Mode</span>
    </div>
    <div class="container">
        <div class="header">
            <h1 class="logo">WebServ</h1>
            <p class="subtitle">User Dashboard</p>
        </div>
        <div class="dashboard-card">
            <div class="welcome-message">✓ Authenticated</div>
            <div class="user-greeting">Welcome back, <?php echo htmlspecialchars($username); ?>!</div>
            <div class="info-grid">
                <div class="info-item">
                    <div class="info-label">Username</div>
                    <div class="info-value"><?php echo htmlspecialchars($username); ?></div>
                </div>
                <div class="info-item">
                    <div class="info-label">Email</div>
                    <div class="info-value"><?php echo htmlspecialchars($email); ?></div>
                </div>
                <div class="info-item">
                    <div class="info-label">Server</div>
                    <div class="info-value"><?php echo htmlspecialchars($connection); ?></div>
                </div>
                <div class="info-item">
                    <div class="info-label">Last Login</div>
                    <div class="info-value"><?php echo htmlspecialchars($loginTime); ?></div>
                </div>
            </div>
            <div class="session-info">
                <div class="session-title">
                    <span class="session-status"></span>
                    Session Active
                </div>
                <div class="session-detail"><strong>Token:</strong> <?php echo substr(htmlspecialchars($token), 0, 16); ?>...</div>
                <div class="session-detail"><strong>Expires:</strong> 1 hour from last activity</div>
                <div class="session-detail"><strong>Storage:</strong> File-based (sessions.csv)</div>
            </div>
            <div class="actions">
                <a href="/" class="btn btn-primary">← Home</a>
                <a href="logout.php" class="btn btn-danger">Logout</a>
            </div>
            <div class="stats">
                <p>🔒 Secure session managed via CGI cookies</p>
                <p>Bonus requirement: Session & Cookie Management ✓</p>
            </div>
        </div>
    </div>
    <script>
        function toggleTheme() {
            const html = document.documentElement;
            const icon = document.getElementById('themeIcon');
            const text = document.getElementById('themeText');
            if (html.getAttribute('data-theme') === 'light') {
                html.removeAttribute('data-theme');
                icon.textContent = '🌙';
                text.textContent = 'Dark Mode';
                localStorage.setItem('theme', 'dark');
            } else {
                html.setAttribute('data-theme', 'light');
                icon.textContent = '☀️';
                text.textContent = 'Light Mode';
                localStorage.setItem('theme', 'light');
            }
        }
        window.addEventListener('DOMContentLoaded', () => {
            const savedTheme = localStorage.getItem('theme');
            if (savedTheme === 'light') {
                document.documentElement.setAttribute('data-theme', 'light');
                document.getElementById('themeIcon').textContent = '☀️';
                document.getElementById('themeText').textContent = 'Light Mode';
            }
        });
    </script>
</body>
</html>
