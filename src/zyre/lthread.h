#ifndef __lzyre_thread_h__
#define __lzyre_thread_h__

#include "zyre.hpp"
#include "lzyre.hpp"
#include "ldb.h"
#include "timeout.h"

#include <boost/asio.hpp>

#include <iostream>
#include <thread>

namespace lzyre {

  class lthread_t
  {
    boost::asio::io_service& ios_;
    boost::asio::io_service::strand strand_;
    std::map<std::string, std::map<std::string, std::shared_ptr<zyre::server>>> files_;
    std::thread T_;

    void run();

  public:
    lthread_t(boost::asio::io_service& aio, const std::map<std::string, std::map<std::string, std::shared_ptr<zyre::server>>>& files);

    void start();
    void join();
  };

}

#endif
