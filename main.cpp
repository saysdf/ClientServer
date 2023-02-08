#include "server.h"
#include "client.h"

using namespace std;
using namespace boost::asio;

int main()
{
    try
    {
        io_service io_service;
        // Start the server on localhost and port 12345
        Server s(io_service, 12345);
        // Start the client and connect to the server
        ip::tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 12345);
       // Client c(io_service, endpoint);
        // Start the event loop
        io_service.run();
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}
