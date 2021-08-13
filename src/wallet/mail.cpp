#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <chrono>
#include <boost/asio.hpp>

#include "zyre/db.hpp"

#include "wallet2.h"
#include "misc_log_ex.h"
#include "cryptonote_config.h"

#include "mail.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.mail"

namespace gyro { namespace sms { namespace mail {

  template<typename T, typename DB>
  class server_t
  {
  public:
    server_t(boost::asio::io_context& io_context, short port, DB& db)
      : db_(db), acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
      do_accept();
    }
  
  private:
    void do_accept()
    {
      acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<T>(std::move(socket), db_)->start();
          }
  
          do_accept();
        });
    }
  
    DB& db_;
    boost::asio::ip::tcp::acceptor acceptor_;
  };
  
  class pop3
    : public std::enable_shared_from_this<pop3>
  {
  public:
    pop3(boost::asio::ip::tcp::socket socket, lldb::cxx::db& db)
      : socket_(std::move(socket))
      , state_(_DEF)
      , db_(db)
    {
    }
  
    void start()
    {
      strcpy(data_, "+OK POP3 server ready\r\n");
      do_write(strlen(data_));
    }
  
  private:
    bool get_message(int n, std::string& content, int& id)
    {
      lldb::cxx::db u = db_[7][user_];
      auto it = u.begin();
      for(; n>1 && it != u.end(); it++)
        { n--; }
      if(it != u.end() && (it->second >> content) &&(it->first >> id))
        return true;
      return false;
    }
    void do_read()
    {
      auto self(shared_from_this());
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (ec) {
              //XLOG(ERROR) << "Error: " << ec.value() << ", " << ec.message();
              do_read();
          }
          //XLOG(WARNING) << std::string(data_, data_+length);
          if(strncmp(data_, "USER", 4) == 0)
          {
            if(length > 5)
            {
              char *a = data_ + 5, *b = user_;
              while (isalnum(*a))
                  *(b++) = *(a++);
              *b = '\0';
              strcpy(data_, "+OK\r\n");
            }
            else
              strcpy(data_, "-ERR\r\n");
            do_write(strlen(data_));
          } else if (strncmp(data_, "PASS", 4) == 0) {
            sprintf(data_, "%s\r\n", "+OK"); // don't care
            do_write(strlen(data_));
          } else if (strncmp(data_, "STAT", 4) == 0) {
            int count = 0, size = 0;
            lldb::cxx::db u = db_[7][user_];
            for (auto m = u.begin(); m != u.end(); m++) {
              count++;
              std::string content;
              m->second >> content;
              size += content.size();
            }
            sprintf(data_, "+OK %d %d\r\n", count, size);
            do_write(strlen(data_));
          } else if (strncmp(data_, "LIST", 4) == 0) {
            state_ = _LIST;
            u_ = db_[7][user_];
            n_ = 0;
            for(it_ = u_.begin(); it_ != u_.end(); it_++)  n_++;
            sprintf(data_, "+OK %d\r\n", n_);
            it_ = u_.begin();
            n_ = 1;
            do_write(strlen(data_));
          } else if (strncmp(data_, "RETR", 4) == 0) {
            if(get_message(atoi(data_ + 4), content_, id_)) {
              state_ = _RETR;
              strcpy(data_, "+OK\r\n");
            } else {
              strcpy(data_, "-ERR\r\n");
            }
            do_write(strlen(data_));
          } else if (strncmp(data_, "DELE", 4) == 0) {
            if(get_message(atoi(data_ + 4), content_, id_)) {
              strcpy(data_, "+OK\r\n");
              db_[7][user_][id_] = nullptr;
            } else
              strcpy(data_, "-ERR\r\n");
            do_write(strlen(data_));
          } else if (strncmp(data_, "TOP ", 4) == 0) {
            sscanf(data_, "TOP %d %d", &n_, &lines_);
            if(get_message(atoi(data_ + 4), content_, id_)) {
              state_ = _TOP;
              strcpy(data_, "+OK\r\n");
              do_write(strlen(data_));
            } else {
              strcpy(data_, "-ERR\r\n.");
              do_write(strlen(data_));
            }
          } else if (strncmp(data_, "QUIT", 4) == 0) {
            state_ = _QUIT;
            strcpy(data_, "+OK\r\n");
            do_write(strlen(data_));
          } else {
            strcpy(data_, "-ERR\r\n");
            do_write(strlen(data_));
          }
        });
    }
    void do_write(std::size_t length)
    {
      auto self(shared_from_this());
      //XLOG(WARNING) << std::string(data_, data_+length);
      boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (ec)
          {
            //XLOG(ERROR) << "Error: " << ec.value() << ", " << ec.message();
            do_read();
            return;
          }
          switch(state_)
          {
          case _LIST:
            {
              if(it_ != u_.end()) {
                it_->second >> content_;
                sprintf(data_, "%d %d\r\n", n_, int(content_.length()));
                it_++; n_++;
              } else {
                strcpy(data_, ".\r\n");
                state_ = _DEF;
              }
              do_write(strlen(data_));
            }
            break;
          case _RETR:
            state_ = _END;
            sprintf(data_, "%s\r\n", content_.data());
            do_write(strlen(data_));
            break;
          case _TOP: {
            state_ = _END;
            size_t n = content_.find("\r\n\r\n");
            if(n != std::string::npos)
            {
              for(int l=0; l<lines_; l++)
                n = content_.find('\n', n);
              if(n != std::string::npos)
                sprintf(data_, "%s\r\n", content_.substr(0, n).data());
              else
                sprintf(data_, "%s\r\n", content_.data());
            }
            else
              sprintf(data_, "%s\r\n", content_.data());
            do_write(strlen(data_));
            }
            break;
          case _END:
            strcpy(data_, ".\r\n");
            state_ = _DEF;
            do_write(strlen(data_));
            break;
          case _QUIT:
            break;
          case _DEF:
          default:
            do_read();
          }
        });
    }
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 8192 };
    enum { _DEF = 0, _LIST, _RETR, _TOP, _END, _QUIT };
    char data_[max_length];
    char user_[1024];
    int id_, lines_;
    std::string content_;
    int state_;
    lldb::cxx::db& db_;
    lldb::cxx::db u_;
    lldb::cxx::db::iterator it_;
    int n_;
  };
  
  class smtp
    : public std::enable_shared_from_this<smtp>
  {
  public:
    smtp(boost::asio::ip::tcp::socket socket, lldb::cxx::db& db)
      : socket_(std::move(socket))
      , db_(db)
      , state_(_DEF)
    {
    }
  
    void start()
    {
      sprintf(data_, "%d %s\r\n", 220, "localhost");
      do_write(strlen(data_));
    }
  
    void recipient_push(char *email)
    {
        std::string recp;
        char *p = strstr(email, "@");
        if(p)
            recp.assign(email, p);
        else
            recp = email;
        recipients_.push_back(recp);
    }
  
  private:
    void do_read()
    {
      auto self(shared_from_this());
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (ec)
          {
            //XLOG(ERROR) << "Error: " << ec.value() << ", " << ec.message();
            return;
          }
          //XLOG(WARNING) << std::string(data_, data_+length);
          //std::cout << std::string(data_, data_+4) << "\n";
  
          if (state_ == _RECV_CONTENT) {
              size_t n = content_.length();
              content_.append(data_, data_+length);
              n = content_.find(".\r\n", n);
              if (n != std::string::npos) {
                for(auto r : recipients_) {
                  int id;
                  if(!(db_["n_private"][r] >> id))
                    id=1;
                  else
                    id++;
                  db_["n_private"][r] = id;
                  db_[7][r][id] = content_;
                }
                state_ = _DEF;
                sprintf(data_, "%d %s\r\n", 250, "OK");
                do_write(strlen(data_));
              }
              else
              {
                sprintf(data_, "%d %s\r\n", 250, "SENDING");
                do_write(strlen(data_));
              }
          } else if (strncmp(data_, "HELO", 4) == 0) {
            sprintf(data_, "%d %s\r\n", 250, "localhost");
            do_write(strlen(data_));

          } else if (strncmp(data_, "RSET", 4) == 0) {
            sprintf(data_, "%d %s\r\n", 250, "OK");
            do_write(strlen(data_));

          } else if (strncmp(data_, "MAIL", 4) == 0) {
            from_ = data_;
            sprintf(data_, "%d %s\r\n", 250, "OK");
            do_write(strlen(data_));

          } else if (strncmp(data_, "RCPT", 4) == 0) {
            if (!from_[0]) {
                sprintf(data_, "%d %s\r\n", 503, "bad sequence");
            } else if (strlen(data_) < 12) {
                sprintf(data_, "%d %s\r\n", 501, "syntax error");
            } else {
                recipient_push(data_ + 9);
                sprintf(data_, "%d %s\r\n", 250, "OK");
            }
            do_write(strlen(data_));

          } else if (strncmp(data_, "DATA", 4) == 0) {
            state_ = _RECV_CONTENT;
            content_.clear();
            sprintf(data_, "%d %s\r\n", 354, "end with .");
            do_write(strlen(data_));

          } else if (strncmp(data_, "QUIT", 4) == 0) {
            sprintf(data_, "%d %s\r\n", 221, "localhost");
            do_write(strlen(data_));
            state_ = _QUIT;
          } else {
            sprintf(data_, "%d %s\r\n", 500, "command unrecognized");
            do_write(strlen(data_));
          }
        });
    }

    void do_write(std::size_t length)
    {
      auto self(shared_from_this());
      //XLOG(WARNING) << std::string(data_, data_+length);
      boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
      [this, self](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (ec)
        {
          //XLOG(ERROR) << "Error: " << ec.value() << ", " << ec.message();
          do_read();
          return;
        }
        if(state_ != _QUIT)
          do_read();
      });
    }

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 8192 };
    char data_[max_length];
    std::string content_;
    std::string from_;
    std::vector<std::string> recipients_;
    lldb::cxx::db& db_;
    enum { _DEF, _RECV_CONTENT, _QUIT };
    int state_;
  };

  server::server(short pop3_port, short smtp_port, const std::string& data_path)
    : pop3_port_(pop3_port)
    , smtp_port_(smtp_port)
    , data_path_(data_path)
    , run_(false)
  {
  }

  void server::set_ports(short pop3_port, short smtp_port)
  {
    pop3_port_ = pop3_port;
    smtp_port_ = smtp_port;
  }
  
  void server::set_path(const std::string& path) {
    data_path_ = path;
  }
 
  void server::run()
  {
    boost::asio::io_context ioc;
    lldb::cxx::db db;
    if(!db.open(data_path_))
    {
      LOG_ERROR("can'not open database: " << data_path_);
      return;
    }

    server_t<pop3, lldb::cxx::db> pop3(ioc, pop3_port_ ? pop3_port_ : 7110, db);
    server_t<smtp, lldb::cxx::db> smtp(ioc, smtp_port_ ? smtp_port_ : 7725, db);

    run_ = true;

    for(;run_;)
      ioc.run_one_for(std::chrono::microseconds(100));

    run_ = false;
  }

  bool server::start()
  {
    if(!run_)
    {
      thread_ = std::thread(std::bind(&zyre::wallet::mail::server::run, this));
      return true;
    }
    return false;
  }

  bool server::stop()
  {
    if(run_)
    {
      run_ = false;
      thread_.join();
      return true;
    }
    return false;
  }

  bool server::is_run()
  {
    return run_;
  };

}}}
