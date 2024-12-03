#include <iostream>
#include "tcp_server.h"


unsigned short port{0};

int main(int argc, char* argv[])
{
    try
    {        
        if (argc != 2)
        {            
            std::cerr << "Usage async_tcp_server: " << argv[0] << " <port>\n";
            return -1;
        }
        
        ba::io_service io_service;

        port = static_cast<unsigned short>(std::stoi(argv[1]));
        tcp::endpoint endpoint(ba::ip::tcp::v4(), port);       
        tcp_server data_server(io_service, endpoint);
        io_service.run();
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
