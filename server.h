#ifndef SERVER_H
#define SERVER_H

#include "session.h"
#include "datacontainer.h"

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;
using boost::asio::ip::tcp;



// Server class to handle incoming connections from clients
class Server
{
public:
    Server(io_service& io_service, short port);
    ~Server();
    void handle_accept(Session* new_session, const boost::system::error_code& error);
    //void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);
    //void handle_send(const boost::system::error_code& error, size_t bytes_transferred);
   
    void check_exit();

private: 
    int collect_value(const int val);
    ip::tcp::acceptor acceptor_;
    //ip::tcp::socket socket_;
    std::list <Session* > sessions_list_;
    boost::asio::io_service& io_service_;
    char data_[sizeof(int)];
    DataContainer container;
    bool stop_;
    Logger log_;
};


#endif // SERVER_H
