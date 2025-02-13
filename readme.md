# 🚀 C++ File Transfer Server

A multithreaded C++ backend for file uploads, downloads, and authentication using native sockets and PostgreSQL.

## 🔥 Features
- ✅ **User Authentication** – Login, signup.
- 📂 **File Upload & Download** – Public/private access, unique download links.
- 🌐 **HTTP Server** – Custom request parsing, `multipart/form-data` support.
- ⚡ **Multithreading** – Efficient client handling.

## 📌 Requirements
- 🔹 C++17+
- 🔹 PostgreSQL
- 🔹 OpenSSL (for secure authentication)

## 🛠 Build & Run
```sh
mkdir build && cd build
cmake ..
make
./http-server [PORT]
