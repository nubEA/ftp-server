#ifndef MULTIPART_PARSER_H
#define MULTIPART_PARSER_H

#include "http_request.h"
#include "util.h"
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iomanip>
#include <arpa/inet.h>

class MultipartParser {
public:
    enum class State {
        ParsingHeaders,
        WritingBody,
        Finished
    };

    MultipartParser(const std::string& boundaryString, const std::string& extra, int fd, bool fullBodyReceived);

private:
    std::string boundary;
    std::vector<unsigned char> extraBody;
    int clientSocketFd;
    State state;

    bool processChunk(std::vector<unsigned char>& chunk, HttpRequest& req, const std::string& finalBoundary, const std::string& endBoundary);
    bool writeToFile(HttpRequest& req, std::vector<unsigned char>& chunk);
    bool writeTextFile(const std::string& tempPath, std::string& content);
    bool writeBinaryFile(const std::string& tempPath, std::vector<unsigned char>& content);
    std::string generateRandomLink();
    void processHeaders(const std::string& headers, HttpRequest& req);
};

#endif // MULTIPART_PARSER_H
