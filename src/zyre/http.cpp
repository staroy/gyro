#include "http.h"
#include "packjson.h"
#include "msgpack.hpp"
#include <boost/algorithm/string.hpp>

namespace http
{

  server::server(boost::asio::io_service& ios, const std::string& host, uint16_t port, const std::string& docroot, const zyre::server& zs)
    : ios_(ios)
    , host_(host)
    , port_(port)
    , docroot_(docroot)
    , name_("http")
    , zyre_(ios_, name_, zs)
  {
    if(docroot_.back() != '/')
        docroot_ += "/";
    ws_.init_asio(&ios_);
    ws_.set_message_handler(websocketpp::lib::bind(&server::on_message,this,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
    ws_.set_http_handler(websocketpp::lib::bind(&server::on_http,this,websocketpp::lib::placeholders::_1));
    ws_.set_open_handler(websocketpp::lib::bind(&server::on_open,this,websocketpp::lib::placeholders::_1));
    ws_.set_close_handler(websocketpp::lib::bind(&server::on_close,this,websocketpp::lib::placeholders::_1));
    ws_.set_validate_handler(websocketpp::lib::bind(&server::on_validate,this,websocketpp::lib::placeholders::_1));
  }

  void server::run()
  {
    zyre_.start();
    zyre_.s_join(name_);
    try
    {
      websocketpp::lib::error_code ec;
      ws_.set_reuse_addr(true);
      ws_.listen(host_, std::to_string(port_), ec);
      if(ec)
        throw std::runtime_error(ec.message());
      ws_.start_accept();
      ws_.run();
    }
    catch (websocketpp::exception const & e)
    {
      MLOG_RED(el::Level::Error, e.what());
    }
    zyre_.s_leave(name_);
    zyre_.stop();
  }

  void server::start()
  {
    thread_ = std::thread(std::bind(&server::run, this));
  }

  void server::join()
  {
    thread_.join();
  }

  void server::on_open(websocketpp::connection_hdl /*hdl*/)
  {
  }

  void server::on_close(websocketpp::connection_hdl /*hdl*/)
  {
  }

  bool server::on_validate(websocketpp::connection_hdl hdl)
  {
    server_t::connection_ptr con = ws_.get_con_from_hdl(hdl);
    const std::vector<std::string> & subp_requests = con->get_requested_subprotocols();
    if (subp_requests.size() > 0)
        con->select_subprotocol(subp_requests[0]);
    return true;
  }

  void server::on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg)
  {
    std::string payload = msg->get_payload();
    std::string pars;
    zyre::json2pack(payload, pars);

    server_t::connection_ptr con = ws_.get_con_from_hdl(hdl);
    zyre::func_t reply = std::bind(
      [con](const std::string& pars)
      {
        size_t off = 0; std::string response;
        zyre::pack2json(pars, off, response);

        con->send(response.c_str());
      },
      std::placeholders::_1
    );

    for (const auto& subp : con->get_requested_subprotocols())
    {
      std::vector<std::string> m;
      boost::split(m, subp, boost::is_any_of("."));
      if(m.size()==2)
        zyre_.do_send_r(inproc_WHISPER, m[0].c_str(), m[1].c_str(), reply, pars);
      else
        MLOG_RED(el::Level::Error, "Error subprotocol name: " << subp << ", <[path_]filename.method> required");
    }
  }

  void server::on_http(websocketpp::connection_hdl hdl)
  {
    server_t::connection_ptr con = ws_.get_con_from_hdl(hdl);

    std::ifstream file;
    std::string filename = con->get_resource();
    std::string response;

    //ws_.get_alog().write(websocketpp::log::alevel::app,
        //"http request1: "+filename);

    if (filename == "/") {
        filename = docroot_+"index.html";
    } else {
        filename = docroot_+filename.substr(1);
    }

    //ws_.get_alog().write(websocketpp::log::alevel::app,
        //"http request2: "+filename);

    file.open(filename.c_str(), std::ios::in);
    if (!file) {
      // 404 error
      std::stringstream ss;

      ss << "<!doctype html><html><head>"
         << "<title>Error 404 (Resource not found)</title><body>"
         << "<h1>Error 404</h1>"
         << "<p>The requested URL " << filename << " was not found on this server.</p>"
         << "</body></head></html>";

      con->set_body(ss.str());
      con->set_status(websocketpp::http::status_code::not_found);
      return;
    }

    file.seekg(0, std::ios::end);
    response.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    response.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    con->set_body(response);
    con->set_status(websocketpp::http::status_code::ok);
  }

}
