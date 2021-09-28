#ifndef __zyre_cc_hpp__
#define __zyre_cc_hpp__

#include "zyre.h"
#include "zyre_pack.hpp"

#include <mutex>
#include <thread>

#include <boost/asio.hpp>

#include "azmq/socket.hpp"

#define VER_MAJOR       0
#define VER_MINOR       1

#define S_JOIN          "S_JOIN"
#define S_LEAVE         "S_LEAVE"
#define JOIN            "JOIN"
#define LEAVE           "LEAVE"
#define REPLY           "REPLY"
#define SHOUT           "SHOUT"
#define WHISPER         "WHISPER"
#define inproc_SHOUT    "inproc.SHOUT"
#define inproc_WHISPER  "inproc.WHISPER"
#define inproc_ZYRE     "inproc://zyre"

#define COOKIE_NEXT     uint64_t(-1)

namespace zyre
{
  class server
  {
    std::thread thread_;
    std::atomic<bool> terminated_;
    std::string name_;
    std::vector<std::string> glist_;
    std::map<std::string, size_t> groups_;
    std::string pin_;
    static void *ctx_;
    bool verbose_;

    void run();
  public:
    server(
      const std::string& name,
      const std::vector<std::string>& groups,
      const std::string& pin = std::string());
    void set_verbose(bool v = true) { verbose_ = v; }
    void start();
    void stop();
    void join();
    static zcert_t *pin_to_cert(const std::string& pin);
    const std::string& name() const { return name_; }
    const std::vector<std::string>& groups() { return glist_; }
    static void *ctx() { return ctx_; }
  };

  class client
  {
    boost::asio::io_service& ios_;
    std::string name_, dev_;
    void *zyre_sock_;
    azmq::socket zyre_;
    std::vector<std::string> groups_;

    static std::atomic<uint64_t> cookie_n_;
    std::map<std::string, func_t> meth_;
    std::map<std::string, func_r_t> meth_r_;
    typedef struct { time_t t; func_t f; } r_info_t;
    std::map<uint64_t, r_info_t> reply_;
    uint8_t reply_timeout_;

    uint64_t do_send(
      const char *zyre_action_p, size_t zyre_action_sz,
      const char *peer_or_group_p, size_t peer_or_group_sz,
      const char *cmd_p, size_t cmd_sz,
      const char *params_p, size_t params_sz,
      uint64_t cookie);

  public:
    client(boost::asio::io_service& ios,
           const std::string& name,
           const server& srv);

    void s_join(const std::string& name);
    void s_leave(const std::string& name);
    void join(const std::string& name);
    void leave(const std::string& name);

    void start();
    void stop();

    const std::vector<std::string>& groups() { return groups_; }

    void do_send(const std::string& zyre_action, const std::string& group, const std::string& cmd, const std::string& pars);
    void do_send_r(const std::string& zyre_action, const std::string& group, const std::string& cmd, const func_t& r, const std::string& pars);
    void do_send(const char *zyre_action, const char *group, char *cmd, const char *pars_p, size_t pars_sz);
    void do_send_r(const char *zyre_action, const char *group, char *cmd, const func_t& r, const char *pars_p, size_t pars_sz);

    void call(const std::string& zyre_action, const std::string& group, const std::string& cmd)
    {
      std::string pars;
      do_send(zyre_action, group, cmd, pars);
    }

    void call(const std::string& zyre_action, const std::vector<std::string>& groups, const std::string& cmd)
    {
      std::string pars;
      for(const auto& group : groups)
        do_send(zyre_action, group, cmd, pars);
    }

    void call_r(const std::string& zyre_action, const std::string& group, const std::string& cmd, const func_t& r)
    {
      std::string pars;
      do_send_r(zyre_action, group, cmd, r, pars);
    }

    void call_r(const std::string& zyre_action, const std::vector<std::string>& groups, const std::string& cmd, const func_t& r)
    {
      std::string pars;
      for(const auto& group : groups)
        do_send_r(zyre_action, group, cmd, r, pars);
    }

    template<typename... A>
    void call(const std::string& zyre_action, const std::string& group, const std::string& cmd, const A&... args)
    {
      std::string pars;
      to_buf(pars, args...);
      do_send(zyre_action, group, cmd, pars);
    }

    template<typename... A>
    void call(const std::string& zyre_action, const std::vector<std::string>& groups, const std::string& cmd, const A&... args)
    {
      std::string pars;
      to_buf(pars, args...);
      for(const auto& group : groups)
        do_send(zyre_action, group, cmd, pars);
    }

    template<typename... A>
    void call_r(const std::string& zyre_action, const std::string& group, const std::string& cmd, const func_t& r, const A&... args)
    {
      std::string pars;
      to_buf(pars, args...);
      do_send_r(zyre_action, group, cmd, r, pars);
    }

    template<typename... A>
    void call_r(const std::string& zyre_action, const std::vector<std::string>& groups, const std::string& cmd, const func_t& r, const A&... args)
    {
      std::string pars;
      to_buf(pars, args...);
      for(const auto& group : groups)
        do_send_r(zyre_action, group, cmd, r, pars);
    }

    void on(const std::string& cmd, const func_t& f);
    void on_r(const std::string& cmd, const func_r_t& f);
  };

}

#endif
