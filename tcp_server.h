#pragma once
#include <array>
#include <set>
#include <mutex>
#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "database.h"

namespace ba = boost::asio;
using boost::asio::ip::tcp;
std::mutex session_mutex;


class session
    : public std::enable_shared_from_this<session>
{
public:
    session(std::shared_ptr<tcp::socket> socket, std::set<std::shared_ptr<session>>& client, std::shared_ptr<db::Database> data) :
        socket_(socket), client_(client), db_(data)
    {
        streambuf_ = std::make_shared<boost::asio::streambuf>();
    }

    void start_session()
    {
        session_mutex.lock();
        client_.insert(shared_from_this());
        session_mutex.unlock();
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        async_read_until(*socket_, *streambuf_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::istream in(streambuf_.get());
                    std::getline(in, str_);
                    do_write();
                }
                else
                {
                    session_mutex.lock();
                    client_.erase(shared_from_this());
                    session_mutex.unlock();
                }
            });
    }

    void do_write()
    {
        str_ = db_->query(std::move(str_));
        str_ += "\n";
        auto self(shared_from_this());
        socket_->async_write_some(boost::asio::buffer(str_),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
                else
                {
                    session_mutex.lock();
                    client_.erase(shared_from_this());
                    session_mutex.unlock();
                }
            });
    }

    std::shared_ptr<tcp::socket> socket_;   
    std::set<std::shared_ptr<session>>& client_;            // ссылка на контейнер с сессиями
    std::shared_ptr<boost::asio::streambuf> streambuf_;     // указатель на буфер для ввода/вывода
    std::shared_ptr<db::Database> db_;                      // указатель на объект БД
    std::string str_;                                       // строка для хранения запроса от клиента и ответа от БД
};


class tcp_server
{
public:
    tcp_server(ba::io_service& io_service, const tcp::endpoint& endpoint)
        : service_(io_service), acceptor_(io_service, endpoint)
    {
        data_= std::make_shared<db::Database>();
        socket_ = std::make_shared<tcp::socket>(service_);
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(*socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::make_shared<session>(socket_, clients_, data_)->start_session();
                }
                socket_ = std::make_shared<tcp::socket>(service_);
                do_accept();
            });
    }

    ba::io_service& service_;
    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> socket_;
    std::set<std::shared_ptr<session>> clients_;
    std::shared_ptr<db::Database> data_;
};
