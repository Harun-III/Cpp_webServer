# 🌐 HTTP: C++ Web Server

This project is an implementation of the **webserv** subject from **42 School**.  
It is a lightweight HTTP/1.0 server written in C++98, using **epoll** for a single non-blocking event loop.

📄 You can find the original subject [here](./docs/Subject.md).

---

## 📂 Project Structure

```plaintext
webserv/
├─ Makefile
├─ conf/                    # config files
│  └─ basic.conf
├─ www/                     # static files for tests
├─ errors/                  # default error pages (html)
├─ bin/                     # simple CGI scripts (python, etc.)
└─ src/
   ├─ config/
   │  ├─ Config.hpp / .cpp          # parse config file → in-memory model
   │  └─ Models.hpp                 # ServerBlock, Location, etc.
   ├─ http/
   │  ├─ Request.hpp / .cpp         # HTTP request structure + helpers
   │  ├─ RequestParser.hpp / .cpp   # parse start-line, headers, body (HTTP/1.0)
   │  ├─ Response.hpp / .cpp        # HTTP response (status, headers, body)
   │  └─ ResponseBuilder.hpp / .cpp # build raw response with Content-Length
   ├─ cgi/
   │  ├─ CgiRunner.hpp / .cpp       # fork+exec, env, pipes (non-blocking)
   │  └─ CgiAdapter.hpp / .cpp      # map Request → CGI → Response
   ├─ server/
   │  ├─ Server.hpp / .cpp          # epoll loop, listeners, connection map
   │  ├─ Listener.hpp / .cpp        # bind/listen (non-blocking)
   │  ├─ Connection.hpp / .cpp      # per-client state machine
   │  └─ Buffer.hpp / .cpp          # safe read/write buffers
   └─ core/
      ├─ Router.hpp / .cpp          # choose handler (static/upload/delete/cgi)
      ├─ Handlers.hpp / .cpp        # Static/Upload/Delete/AutoIndex
      └─ core.cpp                   # glue: wire everything together

NB: Structure Changeable
````

---

## 👥 Team Responsibilities

* **Mehdi** → `config/` + `http/Response.*`
  *(Config parsing, server rules, and building HTTP responses)*

* **Abdelkader** → `server/` + `http/Request.*`
  *(Epoll loop, connections, and HTTP request parsing)*

> ⚠️ **Note:**
> **CGI** is a **shared responsibility** → both teammates must understand it.
> Whichever teammate finishes their main part first will take the lead on implementing CGI.

---

## 🚀 Features (HTTP/1.0 scope)

* Single non-blocking **epoll** loop for all I/O
* Supports **GET**, **POST** (uploads), and **DELETE**
* **Static file serving** with `index` and optional autoindex
* **Config file** inspired by nginx (ports, roots, error pages, methods, uploads, body limits, CGI)
* **Custom error pages** or built-in defaults
* **CGI execution** (e.g., Python scripts) with proper env + stdin/stdout handling
* **Multi-port listening** (different roots per port)
* Accurate **HTTP status codes** (200, 201, 204, 301/302, 400, 403, 404, 405, 413, 500)

---

## 🛠️ Build & Run

```bash
    make && ./webserv conf/webserv.conf
```
