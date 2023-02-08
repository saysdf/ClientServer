#include "client.h"
#include <conio.h>
#include <ctype.h>
#include <thread> 

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;

Client::Client(io_service &io_service, ip::tcp::endpoint &endpoint)
    : socket_(io_service),
      timer_(io_service),
      endpoint_(endpoint),
      stop_(false),
      connection_retry_(3)      
{
    // Connect to the server
    socket_.async_connect(endpoint_, boost::bind(&Client::handle_connect, this,
                                                 boost::asio::placeholders::error));


    
    boost::thread t(boost::bind(&Client::check_exit, this));
    srand(time(0));
}

void Client::handle_connect(const boost::system::error_code &error)
{
    if (!error)
    {
        ip::tcp::endpoint local_endpoint = socket_.local_endpoint();
        log_.set_file("client_log_" + std::to_string(local_endpoint.port()) + ".txt");
        // Generate and send a random number to the server
        int number = rand() % 1024;
        log_ << "[CLIENT]Send number: " << number << "\n";
        boost::asio::async_write(socket_, boost::asio::buffer(&number, sizeof(int)),
                                 boost::bind(&Client::handle_write, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        if (connection_retry_--) {
            cout << "[CLIENT]Can't connect to the server " << endpoint_ << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            cout << "[CLIENT] retry..." << "\n";
            socket_.async_connect(endpoint_, boost::bind(&Client::handle_connect, this,
                boost::asio::placeholders::error));
        }
        else {
            cerr << "Error: " << error.message() << "\n";
        }
    }
}

void Client::handle_write(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (!error)
    {
        // Receive the average of the squares from the server
        boost::asio::async_read(socket_, boost::asio::buffer(data_, sizeof(int)),
                                boost::bind(&Client::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        log_ << "Error: " << error.message() << "\n";
    }
}

void Client::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        // Get the average of the squares of the numbers
        int average = 0;
        memcpy(&average, data_, sizeof(int));
        log_ << "[CLIENT]Average of squares: " << average << "\n";
        // Start the timer for generating a new random number
        timer_.expires_from_now(boost::posix_time::seconds(1));
        timer_.async_wait(boost::bind(&Client::handle_timeout, this,
                                      boost::asio::placeholders::error));
    }
    else
    {
        log_ << "Error: " << error.message() << "\n";
    }
}

void Client::handle_timeout(const boost::system::error_code &error)
{
    if (!error)
    {
        // Generate a new random number
        int number = rand() % 1024;
        log_ << "[CLIENT]Send number: " << number << "\n";
        memcpy(data_, &number, sizeof(int));
        // Send the number to the server
        async_write(socket_, buffer(data_, sizeof(int)),
                    boost::bind(&Client::handle_write, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        log_ << "Error: " << error.message() << "\n";
    }
}

void Client::check_exit() {
    while (!stop_)
    {
        char c = _getch();
        if (c == 27) {
            stop_ = true;
            socket_.close();
        }
    }
}
