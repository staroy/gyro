#include "timeout.h"
#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "zyre.lua.timeout"

namespace lzyre {

  bool timeout::reg(boost::asio::io_service& ios, sol::state_view& lua)
  {
    auto timeout_new = [&](int tm, const sol::protected_function& f) -> std::shared_ptr<timeout> {
      auto ptr = std::make_shared<timeout>(ios, f);
      ptr->wait(tm);
      return ptr;
    };

    lua.new_usertype<timeout>("timeout",
      sol::call_constructor, timeout_new, "new", timeout_new
    );

    return true;
  }

  timeout::timeout(boost::asio::io_service& ios, const sol::protected_function& f)
    : tm_(ios), f_(f)
  {
  }

  void timeout::wait(int t)
  {
    if(t<0)
      return;
    tm_.expires_from_now(boost::posix_time::millisec(uint64_t(t)));
    tm_.async_wait(std::bind(&timeout::on, shared_from_this(), std::placeholders::_1));
  }

  void timeout::on(boost::system::error_code const& ec)
  {
    try
    {
      sol::protected_function_result res = f_();
      if (!res.valid()) {
        sol::error err = res;
        sol::call_status status = res.status();
        MLOG_RED(el::Level::Error, "Lua wrong " << sol::to_string(status) << " error " << err.what());
      }
    }
    catch(const std::exception& e)
    {
      MLOG_RED(el::Level::Error, e.what());
    }
  }
}
