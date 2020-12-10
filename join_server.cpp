#include "boost/asio.hpp"
#include <boost/regex.hpp>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include <iostream>
#include <string>

#include "db.h"


using namespace boost::asio;
using std::cout;
using std::endl;
using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
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

                                        typedef std::vector< std::string > split_vector_type;
                                        split_vector_type commands; // #2: Search for tokens
                                        split(commands, str_data, boost::algorithm::is_any_of("\n") );

                                        commands =
                                                {
                                                "INSERT A 0 lean",
                                                "INSERT A 0 understand",
                                                "INSERT A 1 sweater",
                                                "INSERT A 2 frank",
                                                "INSERT A 3 violation",
                                                "INSERT A 4 quality",
                                                "INSERT A 5 precision",
                                                "INSERT B 3 proposal",
                                                "INSERT B 4 example",
                                                "INSERT B 5 lake",
                                                "INSERT B 6 flour",
                                                "INSERT B 7 wonder",
                                                "INSERT B 8 selection",
                                                "INTERSECTION",
                                                "SYMMETRIC_DIFFERENCE",
                                                "TRUNCATE A",
                                                "TRUNCATE B",
                                                };

                                        const boost::regex insert_filter("INSERT [AB] [0-9]+ [a-zA-Z]+");
                                        const boost::regex trucate_filter("TRUNCATE [AB]");
                                        boost::smatch what;

                                        std::string db_name = "test_database.sqlite";
                                        DB db(db_name);
                                        if (db.open_db() != "OK")
                                        {
                                            return;
                                        }

                                        for (auto & command : commands)
                                        {
//                                            std::cout << command << std::endl;
                                            if (boost::regex_match( command, what, insert_filter ))
                                            {
                                                cout << db.insert_data(command) << endl;
                                            }
                                            else if (boost::regex_match( command, what, trucate_filter))
                                            {
                                                cout << db.truncate_table(command) << endl;
                                            }
                                            else if (command == "INTERSECTION")
                                            {
                                                cout << db.get_intersection() << endl;
                                            }
                                            else if (command == "SYMMETRIC_DIFFERENCE")
                                            {
                                                cout << db.get_symmetric_difference() << endl;
                                            }
                                            else
                                            {
                                                cout << "Unknown command: " << command << endl;
                                            }
                                        }
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
