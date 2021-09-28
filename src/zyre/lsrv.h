#include "zyre.hpp"
#include "auth.h"
#include "http.h"
#include "lthread.h"

#include <boost/asio/io_service.hpp>

namespace lzyre {

class lsrv
{
  boost::asio::io_service& ios_;
  std::unique_ptr<http::server> http_;
  std::map<std::string, std::shared_ptr<zyre::server>> srvs_;
  std::vector<lzyre::lthread_t> S_;
  zactor_t *auth_;

public:
  lsrv(boost::asio::io_service& ios);
  ~lsrv();

  void start(int argc, char *argv[]);
  void stop();
  void join();
};

}