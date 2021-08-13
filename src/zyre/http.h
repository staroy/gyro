#ifndef __zyre_http_h__
#define __zyre_http_h__

#include "wspp.hpp"
#include "zyre.hpp"

namespace http
{

  class server : public  std::enable_shared_from_this<server>
  {
    typedef websocketpp::server<websocketpp::config::server> server_t;
    typedef websocketpp::client<websocketpp::config::client> client_t;

    boost::asio::io_service& ios_;
    server_t ws_;
    std::string host_;
    uint16_t port_;
    std::string docroot_;
    std::thread thread_;

    std::string name_;
    zyre::client zyre_;

  public:
    server(boost::asio::io_service& ios, const std::string& host, uint16_t port, const std::string& docroot, const zyre::server& zs);
    
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    bool on_validate(websocketpp::connection_hdl hdl);
    void on_http(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg);

    void run();

    void start();
    void join();
  };

}

#endif
