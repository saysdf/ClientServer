#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include "logger.h"

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;
// Client class to connect to the server and send random numbers
class Client
{
public:
    Client(io_service& io_service, ip::tcp::endpoint& endpoint);

    void handle_connect(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_timeout(const boost::system::error_code& error);

private:

    void check_exit();
    ip::tcp::socket socket_;
    deadline_timer timer_;
    ip::tcp::endpoint endpoint_;
    bool stop_;
    char data_[sizeof(int)];
    int connection_retry_;
    Logger log_;
};

#endif // CLIENT_H
