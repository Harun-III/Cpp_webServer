#ifndef WEBSERVSTYLE_HPP
# define WEBSERVSTYLE_HPP

// Shared theme style for all directory listing and error pages
# define WEBSERV_STYLE \
    "<style>" \
    "@import url('https://fonts.googleapis.com/css2?family=Fira+Code:wght@300;400;500;700&display=swap');" \
    "*{margin:0;padding:0;box-sizing:border-box}" \
    "body{background:linear-gradient(135deg,#0a0a0a 0%,#1a1a1a 25%,#0d1117 75%,#000 100%);" \
        "color:#00ff41;font-family:'Fira Code',monospace;min-height:100vh}" \
    ".container{max-width:1000px;margin:0 auto;padding:24px}" \
    "h1{margin:0 0 8px;font-weight:700;text-shadow:0 0 12px #00ff41}" \
    "hr{border:0;border-top:1px solid rgba(0,255,65,.3);margin:10px 0 16px}" \
    ".list{display:grid;gap:12px}" \
    ".item a{display:inline-block;color:#00ff41;text-decoration:none;" \
        "border:1px solid #00ff41;padding:6px 12px;border-radius:6px;" \
        "background:linear-gradient(45deg,rgba(0,255,65,.08),rgba(0,255,65,.18));" \
        "font-weight:500;letter-spacing:1px;" \
        "transition:all .3s ease}" \
    ".item a:hover{background:#00ff41;color:#000;box-shadow:0 0 14px rgba(0,255,65,.5)}" \
    "</style></head><body>"

#endif
