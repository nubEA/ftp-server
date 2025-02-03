#include "server.h"
#include "thread_pool.h"
#define BACKLOG 10
#define MAX_BUFFER 1024

//understand why we should throw exceptions over exit(1);

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

Server::Server(char* port) : pool(4){

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
        char buffer[MAX_BUFFER]{};
        int bytesReceived{};
        if((bytesReceived = recv(clientSocketFd, buffer, sizeof(buffer), 0)) == -1){
            perror("Error receiving\n");
            close(clientSocketFd);
            return;                
        }
        else if(bytesReceived == 0)
        {
            std::cout << "Client disconnected\n";
            close(clientSocketFd);
            return;
        }

        std::cout << "Received: " << std::string(buffer, bytesReceived) << '\n';
        //Parse the HTTP request
        HttpRequest req = HttpParser::parse(std::string(buffer,bytesReceived));  
        Router router;
        //Generate response object
        HttpResponse res = router.handle_request(req);
        //Get string response
        std::string string_res = res.get_string_response();

        if(send(clientSocketFd,string_res.c_str(),string_res.size(),0) == -1){
            perror("Error Sending\n");
        }
        close(clientSocketFd);
        std::cout << "Connection closed\n";
    };

    pool.submit(clientTaskHandler);
}

