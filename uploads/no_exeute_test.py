#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html\r\n\r\n")
print("<html><head><title>CGI Test</title></head><body>")
print("<h1>Hello from CGI!</h1>")
print("<p>This is a simple Python CGI script.</p>")

print("<h2>Environment Variables:</h2><ul>")
for key, value in sorted(os.environ.items()):
    if key.startswith(('HTTP_', 'REQUEST_', 'SERVER_', 'CONTENT_', 'QUERY_')):
        print(f"<li><strong>{key}:</strong> {value}</li>")
print("</ul>")

print("<h2>Request Method:</h2>")
print(f"<p>{os.environ.get('REQUEST_METHOD', 'Unknown')}</p>")

if os.environ.get('REQUEST_METHOD') == 'POST':
    print("<h2>POST Data:</h2>")
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        print(f"<pre>{post_data}</pre>")

print("</body></html>")
