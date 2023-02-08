#pragma once

#include <iostream>
#include <set>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <functional>
#include "logger.h"

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;
using boost::asio::ip::tcp;



class Session
{
public:
    Session(Logger& log, boost::asio::io_service& io_service, std::function<int(const int)> collecting_function)
        : log_(log), socket_(io_service),
        functional_pointer(collecting_function)
    {

    }
    ~Session() {

    }
    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error && bytes_transferred > 0)
        {
            // Get the endpoint of the remote client
            auto remote_endpoint = socket_.remote_endpoint();
            log_ << "   [SOCKET] Received data from client " << remote_endpoint << "\n";
            // Store the unique number in the container
            int number = *reinterpret_cast<int*>(data_);
            int average = functional_pointer(number);
            // Send the average back to the client
            boost::asio::async_write(socket_,
                boost::asio::buffer(&average, sizeof(int)),
                boost::bind(&Session::handle_write, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            log_ << "   [SOCKET] close." << "\n";
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&Session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            log_ << "   [SOCKET] close." << "\n";
            delete this;
        }
    }


private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    Logger& log_;
    std::function<int(const int)> functional_pointer;
};
