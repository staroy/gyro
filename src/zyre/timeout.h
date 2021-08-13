#ifndef __lzyre_timeout_h__
#define __lzyre_timeout_h__

#include "sol/sol.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace lzyre {

  class timeout : public  std::enable_shared_from_this<timeout>
  {
    boost::asio::deadline_timer tm_;
    sol::protected_function f_;
  public:
    timeout(boost::asio::io_service& ios, const sol::protected_function& f);
    void wait(int t);
    void on(boost::system::error_code const& ec);
    static bool reg(boost::asio::io_service& ios, sol::state_view& lua);
  };

}

#endif