#!/usr/bin/php
<?php
header("Content-Type: text/html; charset=utf-8");

echo "<html><head><title>CGI Test</title></head><body>";
echo "<h1>Hello from PHP CGI!</h1>";
echo "<p>This is a simple PHP CGI script.</p>";

echo "<h2>Environment Variables:</h2><ul>";
foreach ($_SERVER as $key => $value) {
    if (preg_match('/^(HTTP_|REQUEST_|SERVER_|CONTENT_|QUERY_)/', $key)) {
        echo "<li><strong>" . htmlspecialchars($key) . ":</strong> " . htmlspecialchars($value) . "</li>";
    }
}
echo "</ul>";

echo "<h2>Request Method:</h2>";
$method = $_SERVER['REQUEST_METHOD'] ?? 'Unknown';
echo "<p>$method</p>";

if ($method === 'POST') {
    echo "<h2>POST Data:</h2>";
    echo "<p>Reading all stdin:</p>";
    $post_data = file_get_contents("php://input");
    echo "<pre>" . htmlspecialchars($post_data) . "</pre>";
}

echo "</body></html>";
?>

