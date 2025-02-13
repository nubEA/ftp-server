#ifndef MULTIPART_PARSER_H
#define MULTIPART_PARSER_H

#include "http_request.h" // For HttpRequest class
#include "util.h"         // For utility functions
#include <string>         // For std::string
#include <vector>         // For std::vector
#include <unistd.h>       // For POSIX operating system API (e.g., read, write)
#include <sys/types.h>    // For data types used in system calls
#include <sys/socket.h>   // For socket programming
#include <sys/select.h>   // For select system call (I/O multiplexing)
#include <netinet/in.h>   // For internet protocol suite
#include <netdb.h>        // For network database operations (e.g., gethostbyname)
#include <filesystem>     // For filesystem operations (C++17 and later)
#include <iomanip>        // For stream manipulators (e.g., std::setprecision)
#include <arpa/inet.h>    // For internet address conversion functions

class MultipartParser
{
public:
    enum class State
    { // Enum to represent the parsing state
        ParsingHeaders,
        WritingBody,
        Finished
    };

    MultipartParser(const std::string &boundaryString, const std::string &extra, int fd,
                    bool fullBodyReceived, HttpRequest::UploadedFile &file); // Constructor

private:
    std::string boundary;                 // Boundary string for multipart parsing
    std::vector<unsigned char> extraBody; // Any extra data received before the boundary
    int clientSocketFd;                   // File descriptor for the client socket
    State state;                          // Current state of the parser

    bool processChunk(std::vector<unsigned char> &chunk, HttpRequest &req, const std::string &finalBoundary, const std::string &endBoundary); // Processes a chunk of data
    bool writeToFile(HttpRequest &req, std::vector<unsigned char> &chunk);                                                                    // Writes the chunk to a file
    bool writeTextFile(const std::string &tempPath, std::string &content);                                                                    // Writes the content of text files
    bool writeBinaryFile(const std::string &tempPath, std::vector<unsigned char> &content);                                                   // Writes content of binary files
    std::string generateRandomLink();                                                                                                         // Generates a random link
    void processHeaders(const std::string &headers, HttpRequest &req);                                                                        // Parses the headers
};

#endif // MULTIPART_PARSER_H