#include "server.h"

#include <conio.h>
#include <ctype.h>

using namespace std;
using namespace boost::asio;
using namespace boost::placeholders;


/*Server::Server(io_service& io_service, short port)
    : acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), port)),
      socket_(io_service),
      stop_(false)
{*/

Server::Server(boost::asio::io_service & io_service, short port)
        : io_service_(io_service),
          acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), port)),
          stop_(false),
          log_("server_log.txt"),
          container(log_)
{
   // Start listening for incoming connections
    Session* new_session = new Session(log_,io_service_, std::bind(&Server::collect_value,this,std::placeholders::_1));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
//    acceptor_.async_accept(socket_, boost::bind(&Server::handle_accept, this,
//                                                boost::asio::placeholders::error));
    // Start the exit function to check the ESC button
    boost::thread t(boost::bind(&Server::check_exit, this));
    log_ << "[SERVER] Started. " << char(1) << "\n";

    container.dump("dump.txt");
}

Server::~Server() {
    log_ << "[SERVER] Stoped. " << char(1) << "\n";
}

void Server::handle_accept(Session* new_session, const boost::system::error_code &error)
{
    if (!error)
    {
        sessions_list_.push_back(new_session);
        new_session->start();
        new_session = new Session(log_,io_service_, std::bind(&Server::collect_value, this, std::placeholders::_1));
        acceptor_.async_accept(new_session->socket(),
                                boost::bind(&Server::handle_accept, this, new_session,
                                            boost::asio::placeholders::error));
    }
    else if (!stop_)
    {
        delete new_session;
        // Start listening for the next incoming connection
        //acceptor_.async_accept(socket_, boost::bind(&Server::handle_accept, this,
        //    boost::asio::placeholders::error));
    }
}

int Server::collect_value(const int val) {
    return container.add(val);
}

void Server::check_exit() {
    while (!stop_)
    {
        char c = _getch();
        if (c == 27) {
            stop_ = true;
            log_ << "[SERVER] acceptor stop..." << "\n";
            acceptor_.close();
            log_ << "[SERVER] acceptopr stopeed." << "\n";
            log_ << "[SERVER] closing all sessions... "  << "\n";
            for (auto session : sessions_list_) {
                session->socket().close();
            }
            sessions_list_.clear();
            log_ << "[SERVER] all session closed." << "\n";
            log_ << "[SERVER] exit." << "\n";
        }
    }
}