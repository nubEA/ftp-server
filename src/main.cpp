#include <iostream>
#include "server.h" // Include the Server class definition

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    char* port = argv[1];

    Server server(port); 

    return 0;
}