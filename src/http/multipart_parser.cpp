#include "multipart_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <openssl/rand.h>
#include <fcntl.h>
#include <unistd.h>
#include <filesystem>

#define MAX_BUFFER 64 * 1024

MultipartParser::MultipartParser(const std::string& boundaryString, const std::string& extra, int fd, bool fullBodyReceived, HttpRequest::UploadedFile& file)
    : boundary(boundaryString), extraBody(extra.begin(), extra.end()), clientSocketFd(fd), state(State::ParsingHeaders) {

    std::cout << "MultipartParser constructor called\n";
    std::vector<unsigned char> chunk = extraBody;
    std::vector<unsigned char> buff(MAX_BUFFER);
    ssize_t bytesReceived{-1};
    HttpRequest multipartBody;
    std::string FinalBoundary = "\r\n--" + boundary + "--";
    std::string endBoundary = "\r\n--" + boundary + "\r\n";

    std::vector<std::string> endBoundaryPerms{};
    std::vector<std::string> finalBoundaryPerms{};
    for (size_t i = 1; i < endBoundary.length(); ++i)
    {
        endBoundaryPerms.push_back(endBoundary.substr(i));
        finalBoundaryPerms.push_back(FinalBoundary.substr(i));
    }

    std::cout << "@@@@@@@@@@@@@@@\n\n";
    for (auto& v : chunk) {
        std::cout << v;
    }
    std::cout << "\n\n";

    if (fullBodyReceived) {
        if (!processChunk(chunk, multipartBody, FinalBoundary, endBoundary)) {
            std::cerr << "Error processing chunk. Closing connection.\n";
            return;
        }
        if (state == MultipartParser::State::Finished) {
            std::cout << "State is Finished, writing to file\n";
            writeToFile(multipartBody, chunk);
        }
        return;
    }

     bool wait = false;

    while ((bytesReceived = recv(clientSocketFd, buff.data(), MAX_BUFFER, 0)) > 0) {
        std::cout << "Received " << bytesReceived << " bytes\n";

        chunk.insert(chunk.end(), buff.begin(), buff.begin() + bytesReceived);

        auto finalBoundaryIt = std::search(chunk.begin(), chunk.end(), FinalBoundary.begin(), FinalBoundary.end());
        auto endBoundaryIt = std::search(chunk.begin(), chunk.end(), endBoundary.begin(), endBoundary.end());

          if (finalBoundaryIt != chunk.end()) { // Check final boundary here
            size_t finalPos = std::distance(chunk.begin(), finalBoundaryIt);
            std::vector<unsigned char> bodyPartChunk(chunk.begin(), chunk.begin() + finalPos);
            chunk.erase(chunk.begin(), chunk.begin() + finalPos + FinalBoundary.length());
              if (!processChunk(bodyPartChunk, multipartBody, FinalBoundary, endBoundary)) {
                  std::cerr << "Error processing chunk. Closing connection.\n";
                  return;
              }
            state = State::Finished; // Set finished here.
             if (state == MultipartParser::State::Finished) {
                std::cout << "State is Finished, writing to file\n";
                writeToFile(multipartBody, chunk);
                break; // Exit recv loop.
            }
        }
        else if (endBoundaryIt != chunk.end()) {
            size_t endPos = std::distance(chunk.begin(), endBoundaryIt);

            std::vector<unsigned char> bodyPartChunk(chunk.begin(), chunk.begin() + endPos);
            chunk.erase(chunk.begin(), chunk.begin() + endPos + endBoundary.length());
            if (!processChunk(bodyPartChunk, multipartBody, FinalBoundary, endBoundary)) {
                std::cerr << "Error processing chunk. Closing connection.\n";
                return;
            }

            state = State::ParsingHeaders; // Reset to parsing headers for the *next* part.
        }
        else {
             size_t fromBack = std::min(chunk.size(),FinalBoundary.length());
            std::string lookBack(chunk.end() - fromBack, chunk.end());

            for(int i = 0; i < endBoundaryPerms.size(); ++i)
            {
                std::string str1 = lookBack + endBoundaryPerms[i];
                std::string str2 = lookBack + finalBoundaryPerms[i];
                if((str1.find(endBoundary) != std::string::npos) || (str2.find(FinalBoundary) != std::string::npos))
                {
                    std::cout << "Going to wait for another receive call....\n";
                    wait = true;
                    break;
                }
            }

            if(wait){
                wait = false;
                continue;
            }
            if (!processChunk(chunk, multipartBody, FinalBoundary, endBoundary)) {
                std::cerr << "Error processing chunk. Closing connection.\n";
                return;
            }
        }
    }

    if (bytesReceived == -1) {
        perror("Error receiving");
        close(clientSocketFd);
        return;
    }

    std::cout << "Filling in the file struct now to pass on to the router\n";
    file = multipartBody.file;
}

bool MultipartParser::processChunk(std::vector<unsigned char>& chunk, HttpRequest& req, const std::string& finalBoundary, const std::string& endBoundary) {
    std::cout << "Processing chunk\n";

    if (state == State::Finished) {
        return true;
    }

    auto finalBoundaryIt = std::search(chunk.begin(), chunk.end(), finalBoundary.begin(), finalBoundary.end());
    if (finalBoundaryIt != chunk.end()) { // Check final boundary here too.
        state = State::Finished;
        return true;
    }

    // *** IMPORTANT FIX: Reset headerEndIt search to start from chunk.begin() ***
    auto headerEndIt = std::search(chunk.begin(), chunk.end(), std::begin("\r\n\r\n"), std::end("\r\n\r\n") - 1);
    if (headerEndIt != chunk.end()) {
        std::cout << "Header end found\n";
        size_t headerEndPos = std::distance(chunk.begin(), headerEndIt);
        std::string entireHeader(chunk.begin(), chunk.begin() + headerEndPos);
        chunk.erase(chunk.begin(), chunk.begin() + headerEndPos + 4);
        processHeaders(entireHeader, req);
    }

    if (state == State::WritingBody) {
        std::cout << "Writing body\n";
        if (!writeToFile(req, chunk)) {
            std::cerr << "Error saving file locally. Closing connection.\n";
            return false;
        }
    }
    return true;
}

bool MultipartParser::writeToFile(HttpRequest& req, std::vector<unsigned char>& chunk) {
    std::cout << "Writing to file\n";
    std::string hashedFileName = "/home/harshit/code/http-server/uploads/" + req.file.downloadLink;
    std::string tempFile = hashedFileName + ".tmp";


    if (state == MultipartParser::State::Finished) {
        std::cout << "Renaming temp file to final file\n";

        double fileSizeMB = static_cast<double>(std::filesystem::file_size(tempFile)) / (1024 * 1024);
        req.file.size = fileSizeMB;

        if (std::rename(tempFile.c_str(), hashedFileName.c_str()) != 0) {
            std::cerr << "Error renaming temp file to final file\n";
            std::remove(tempFile.c_str());
            return false;
        }
        std::cout << "File successfully renamed: " << hashedFileName << "\n";
        return true;
    }


    std::cout << "Writing binary file\n";
    if (!writeBinaryFile(tempFile, chunk)) {
        std::cerr << "Binary file writing failed.\n";
        std::remove(tempFile.c_str());
        return false;
    }

    return true;
}

bool MultipartParser::writeBinaryFile(const std::string& tempPath, std::vector<unsigned char>& content) {
    std::cout << "Writing binary content to file: " << tempPath << "\n";
    std::ofstream filestream(tempPath, std::ios::binary | std::ios::app);
    if (!filestream.is_open()) {
        std::cerr << "Error opening binary file: " << tempPath << "\n";
        return false;
    }

    std::string bodyEnd = "\r\n--" + boundary + "\r\n";
    std::string finalString = "--" + boundary + "--";

    auto endPosIt = std::search(content.begin(), content.end(), bodyEnd.begin(), bodyEnd.end());
    auto finalPosIt = std::search(content.begin(), content.end(), finalString.begin(), finalString.end());
    size_t endPos = content.size();

    if (finalPosIt != content.end()) {
        endPos = std::distance(content.begin(), finalPosIt);
        state = MultipartParser::State::Finished; // Set the final state
    } else if (endPosIt != content.end()) {
        endPos = std::distance(content.begin(), endPosIt);
    }

    filestream.write(reinterpret_cast<char*>(content.data()), endPos);

    if (endPos == content.size()) {
        content.clear();
    } else {
        // Corrected erase logic:
        if (finalPosIt != content.end()) {
            content.erase(content.begin(), content.begin() + endPos + finalString.length());
        } else if (endPosIt != content.end()){ //if not final boundary check for the intermediate one
            content.erase(content.begin(), content.begin() + endPos + bodyEnd.length());
        } else{ //If nothing found just erase the chunk
            content.erase(content.begin(), content.begin() + endPos);
        }
    }

    filestream.flush();
    if (filestream.fail()) {
        std::cerr << "Error flushing the file: " << tempPath << "\n";
        filestream.close();
        return false;
    }

    int fd = open(tempPath.c_str(), O_RDWR);
    if (fd == -1) {
        std::cerr << "Error opening file descriptor for syncing: " << tempPath << "\n";
        filestream.close();
        return false;
    }

    if (fsync(fd) == -1) {
        std::cerr << "Error syncing file to disk: " << tempPath << "\n";
        filestream.close();
        close(fd);
        return false;
    }

    close(fd);
    filestream.close();
    std::cout << "Closed temp file after writing binary content\n";
    return true;
}

std::string MultipartParser::generateRandomLink() {
    std::cout << "Generating random link\n";
    unsigned char buff[8];
    RAND_bytes(buff, 8);
    std::stringstream stream;
    std::string link;
    for (auto& v : buff) {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(v);
    }
    link = stream.str();
    return link;
}
void MultipartParser::processHeaders(const std::string& headers, HttpRequest& req) {
    std::cout << "Raw headers:\n" << headers << "\n";

    std::stringstream stream(headers);
    std::unordered_map<std::string, std::string> headerMap;
    std::string line;

    while (std::getline(stream, line)) {
        size_t colonPos = line.find(":");
        if (colonPos == std::string::npos) {
            continue; // Skip lines without a colon
        }

        std::string key = Util::to_lower(line.substr(0, colonPos));
        key = Util::trim(key);
        std::string value = Util::trim(line.substr(colonPos + 1));

        // Always store headers *before* processing Content-Disposition
        headerMap[key] = value;
        std::cout << "Header parsed: " << key << ": " << headerMap[key] << std::endl;


        if (Util::to_lower(key) == "content-disposition") {
            std::cout << "Found Content-Disposition header: " << value << std::endl;

            size_t namePos = value.find("name=\"");
            if (namePos != std::string::npos) {
                namePos += 6;
                size_t nameEnd = value.find("\"", namePos);
                std::string name = value.substr(namePos, nameEnd - namePos);
                std::cout << "Extracted name: " << name << std::endl;

                if (name == "public") {
                    std::cout << "################## Found public header! ####################\n";
                    req.file.perms = "public";
                    // NO state change here.
                    continue; // Correct: Skip to the next header line
                }
            }

            size_t filenamePos = value.find("filename=\"");
            if (filenamePos != std::string::npos) {
                filenamePos += 10;
                size_t filenameEnd = value.find("\"", filenamePos);
                std::string filename = value.substr(filenamePos, filenameEnd - filenamePos);
                std::cout << "Extracted filename: " << filename << std::endl;

                HttpRequest::FileType type = HttpRequest::FileType::BINARY; //Default is binary
                if (headerMap.count("content-type") && headerMap.at("content-type").starts_with("text/")) {
                    std::cout << "Setting File type as text-only...\n";
                    type = HttpRequest::FileType::TEXT;
                } else {
                    std::cout << "Setting File content type as binary\n";
                }

                req.file.filename = filename;
                req.file.type = type;
                req.file.downloadLink = generateRandomLink();
                state = State::WritingBody; // Correct: Transition to WritingBody *only* when a filename is found.
            }
        }
    }
}