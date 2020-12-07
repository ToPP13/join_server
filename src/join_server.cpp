#include "boost/asio.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "../include/lib.h"
#include <vector>

using namespace boost::asio;
using std::cout;
using std::endl;

using boost::asio::ip::tcp;

class Session
        : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket socket)
            : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length)
                                {
                                    if (!ec)
                                    {
                                        std::string str_data = std::string{data_, length};
                                    }
                                });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server
{
public:
    explicit Server(short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
    }

    void run() {
        do_accept();
        io_context.run();
    }

private:
    boost::asio::io_context io_context;

    void do_accept()
    {
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        std::make_shared<Session>(std::move(socket))->start();
                    }

                    do_accept();
                });
    }

    tcp::acceptor acceptor_;
};




int main(int argc, char *argv[])
{
    short port = 3;
    if (argc != 2)
    {
        cout << "no port!" << endl;
    }
    else
    {
        std::istringstream iss( argv[1] );
        if (!(iss >> port))
        {
            cout << "Incorrect port!" << endl;
            return -1;
        }
    }

    Server server(port);
    server.run();
    return 0;
}
