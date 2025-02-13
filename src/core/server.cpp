#include "server.h"

#define BACKLOG 10
#define MAX_BUFFER 65536 //64 KB

static Server* serverInstance = nullptr;

void signalHandler(int signum)
{   
    std::cout << "Interrupt signal " << signum << " received\n";
    if(serverInstance){
        serverInstance->stop();
        char message[] = "Q";
        if(write(serverInstance->pipefd[1], message,strlen(message)+1) == -1)
        {
            perror("Error writing to pipe in signal handler\n");
        }
    }
}

Server::Server(char* port): 
pool(4),
connPool(5, std::string(std::getenv("DB_CONNECTION_STRING") ? std::getenv("DB_CONNECTION_STRING") : throw std::runtime_error("DB_CONNECTION_STRING not set")), 5000),
db(connPool)
{
    set_up_signal_handler();

    int sockfd = create_bind_socket(port);
    start_listening(sockfd, port);

    while(!shutServer.load())
    {
        handle_request(sockfd);
    }

    std::cout << "Shutting down the server now, Bye!\n";
    close(sockfd);
    pool.shutdown();
}

Server::~Server(){
    if(!shutServer.load()) stop();
    close(pipefd[0]);
    close(pipefd[1]);
    serverInstance = nullptr;   //to avoid dangling pointer
}

void Server::stop(){
    shutServer = true;
}

void Server::set_up_signal_handler(){
    if(pipe(pipefd) == -1){
        perror("Error in creating a pipe\n");
        throw std::runtime_error("Failed to create the pipe");
    }
    serverInstance = this;
    std::signal(SIGINT, signalHandler);         //defines what to do when the OS sends sigint signal which comes on pressing ctrl + c;
}

int Server::create_bind_socket(char* port){
    int sockfd{}; 
    struct addrinfo hints{};
    struct addrinfo* listOfAddr{};
    struct addrinfo* p{};

    hints.ai_family = AF_UNSPEC;            //dont care about IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;        //TCP socket
    hints.ai_flags = AI_PASSIVE;            //use my IP

    int rv{};
    if((rv = getaddrinfo(NULL,port,&hints,&listOfAddr)) != 0){
        throw std::runtime_error("Error getting addr info\n");
    }

    for(p = listOfAddr; p != nullptr; p = p->ai_next)
    {
        if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
        {
            perror("Error creating a socket\n");
            continue;
        }

        if(bind(sockfd,p->ai_addr,p->ai_addrlen) < 0)
        {
            perror("Error binding socket\n");
            close(sockfd);
            continue;
        }
        break;
    }


    if(!p){
        freeaddrinfo(listOfAddr);
        throw std::runtime_error("Error in binding\n");
    }
    freeaddrinfo(listOfAddr);
    return sockfd;
}

void Server::start_listening(int sockfd, char* port)
{
    if(listen(sockfd,BACKLOG) < 0){
        throw std::runtime_error("Error in listening\n");
    }

    std::cout << "The server has started listening on port: " << port << '\n';
}

int Server::accept_connection(int sockfd)
{
    struct sockaddr_in clientAddr{};
    socklen_t clientAdrrLen{sizeof(clientAddr)};

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);                   //clearing out the set
    FD_SET(sockfd,&current_sockets);            //pushing sockfd into the set
    FD_SET(pipefd[0],&current_sockets);
    int maxFd = std::max(sockfd,pipefd[0]);
    ready_sockets = current_sockets;            //this will be overwritten by the select() call

    if(select(maxFd+1,&ready_sockets,NULL,NULL,NULL) < 0){
        //errno gets set to EINTR when select is interrupted (like our ctrl+c signal)
        if(errno == EINTR) return -1;

        throw std::runtime_error("Select Error\n");
    }

    for(int i = 0; i <= maxFd; ++i)
    {
        if(FD_ISSET(i,&ready_sockets)){
            if(i == sockfd){
                int clientSocketFd = accept(sockfd,(struct sockaddr*)&clientAddr,&clientAdrrLen); 

                if(clientSocketFd < 0){
                    throw std::runtime_error("Error accepting connection\n");
                }

                std::cout << "Accepted Connection\n";

                return clientSocketFd;
            }
            else{
                return -1; //-1 returned to indicate shutdown
            }
        }
    }
    return -1;
}

void Server::handle_request(int sockfd)
{
    int clientSocketFd = accept_connection(sockfd);

    if(clientSocketFd == -1) return;          

    //lambda function
    auto clientTaskHandler = [this, clientSocketFd] () {
        std::vector<char> buffer(MAX_BUFFER);
        std::string raw_request{};
        bool isMultipart = false;
        size_t headerEnd = std::string::npos;
        int bytesReceived{-1};
        //Getting the headers in chunks (may include some of body too)
        while((bytesReceived = recv(clientSocketFd, buffer.data(), MAX_BUFFER, 0)) > 0)
        {
            raw_request.append(buffer.data(),bytesReceived);
            //If all headers have been received
            headerEnd = raw_request.find("\r\n\r\n");
            if(headerEnd != std::string::npos)
            {
                break;
            }
        }

        if(headerEnd != std::string::npos){
            std::cout << "\n\n";
            std::cout << "###\n";
            std::cout << raw_request << "\n\n\n";
            std::cout << raw_request.substr(0,headerEnd) << "\n\n";
            size_t multipartPos = raw_request.find("multipart");
            isMultipart = (multipartPos != std::string::npos && multipartPos < headerEnd);
            std::cout << isMultipart << "\n\n";
        }
        //Multipart/form data including file so we call the streaming file writer
        HttpRequest req = HttpParser::parse(raw_request);
        bool bodyReceived = req.full_body_received();

        if(isMultipart)
        {
            //Parse the headers and some body that may have come
            std::cout << "Sending the request to multipart parser\n";
            std::string ExtraBody = req.get_body();
            std::string boundaryString = req.get_boundary_string();

            MultipartParser mParser(boundaryString, ExtraBody, clientSocketFd, bodyReceived, req.file);
        }
        //Normal login/signup/get requests
        else{
            while(!bodyReceived && (bytesReceived = recv(clientSocketFd, buffer.data(), MAX_BUFFER, 0)) > 0)
            {
                std::cout << "Receiving the rest of the body....\n";
                raw_request.append(buffer.data(),bytesReceived);
                //Since raw request contains headers too
                //We use headerEnd pos + 4 (offset for \r\n\r\n) to only store body
                req.update_body(raw_request.substr(headerEnd+4));
                bodyReceived = req.full_body_received();
            }
            std::cout << "Received full body now\n";
        }

        Router router;
        HttpResponse res = router.handle_request(req,db,clientSocketFd);
        
        if(!req.get_path().starts_with("download/")){
            std::string string_res = res.get_string_response();
            if(send(clientSocketFd,string_res.c_str(),string_res.size(),0) == -1){
                perror("Error sending response\n");
            }
        }

        close(clientSocketFd);
        std::cout << "Connection Closed\n";
    };

    pool.submit(clientTaskHandler);
}

Database& Server::get_database(){
    return db;
}

