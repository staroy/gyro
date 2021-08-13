#include "zyre.hpp"
#include "auth.h"

#include <iostream>

#include <boost/filesystem.hpp>

#include "misc_log_ex.h"
#include "sodium.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "zyre.cc"

namespace zyre
{

  void *server::ctx_ = nullptr;

  server::server(
    const std::string& name,
    const std::vector<std::string>& groups,
    const std::string& pin)
    : name_(name)
    , glist_(groups)
    , pin_(pin)
    , verbose_(false)
  {

    if(!ctx_)
      ctx_ = zsys_init();

    terminated_ = false;

    for(const auto& g : groups)
      groups_[g] = 0;
  }

  void server::start()
  {
    thread_ = std::thread(std::bind(&server::run, this));
  }

  void server::stop()
  {
    terminated_ = true;
  }

  void server::join()
  {
    thread_.join();
  }

  void server::run()
  {
    std::set<std::string> scripts;

    zyre_t *node = zyre_new( name_.c_str() );
    if (!node)
    {
      MLOG_RED(el::Level::Warning, "Could not create new zyre node");
      return;
    }

    if(!pin_.empty())
    {
      zyre_set_zap_domain(node, ("zap-" + name_).c_str());

      zcert_t *cert = pin_to_cert(pin_);

      zyre_set_zcert(node, cert);
      zyre_set_header(node, "X-PUBLICKEY", "%s", zcert_public_txt(cert));

      if(verbose_)
        zyre_set_verbose(node);

      zcert_destroy(&cert);
    }

    zyre_start(node);

    for(const auto& g : groups_)
      zyre_join(node, g.first.c_str());

    // connection from inproc clients
    zsock_t *inproc = zsock_new_router((inproc_ZYRE "-" + name_).c_str());
    zpoller_t *poller = zpoller_new(zyre_socket(node), inproc, NULL);

    while(!terminated_)
    {
      void *which = zpoller_wait(poller, 100);

      try
      {
        if(which == inproc)
        {
          zmsg_t *msg = zmsg_recv(which);
          if (!msg) {
            MLOG_RED(el::Level::Error, "Interrupted zyre node");
            break;
          }
        
          zframe_t *src = zmsg_pop(msg);
          zframe_t *op = zmsg_pop(msg);
        
          if(zframe_streq(op, S_JOIN))
          {
            char *name = zmsg_popstr(msg);
            auto s = scripts.find(name);
            if(s == scripts.end())
              scripts.insert(name);
            zstr_free(&name);
          }
          else if(zframe_streq(op, S_LEAVE))
          {
            char *name = zmsg_popstr(msg);
            auto s = scripts.find(name);
            if(s != scripts.end())
              scripts.erase(s);
            zstr_free(&name);
          }
          else if(zframe_streq(op, JOIN))
          {
            char *name = zmsg_popstr(msg);
            auto g = groups_.find(name);
            if(g == groups_.end())
            {
              groups_[name] = 1;
              zyre_join(node, name);
            }
            else
              g->second++;
            zstr_free(&name);
          }
          else if(zframe_streq(op, LEAVE))
          {
            char *name = zmsg_popstr(msg);
            auto g = groups_.find(name);
            if(g != groups_.end())
            {
              if(g->second == 1)
              {
                groups_.erase(g);
                zyre_leave(node, name);
              }
              if(g->second > 1)
                g->second--;
            }
            zstr_free(&name);
          }
          else if(zframe_streq(op, inproc_SHOUT) || zframe_streq(op, inproc_WHISPER))
          {
            zframe_t *dst = zmsg_pop(msg);
            zmsg_prepend(msg, &src);
            zmsg_prepend(msg, &op);
            zmsg_prepend(msg, &dst);
            zmsg_send(&msg, inproc);
          }
          else if(zframe_streq(op, SHOUT))
          {
            char *group = zmsg_popstr(msg);
            zmsg_prepend(msg, &src);
            zyre_shout(node, group, &msg);
            zstr_free(&group);
          }
          else if(zframe_streq(op, WHISPER))
          {
            char *peer = zmsg_popstr(msg);
            zmsg_prepend(msg, &src);
            zyre_whisper(node, peer, &msg);
            zstr_free(&peer);
          }
          else
          {
            MLOG_RED(el::Level::Error, "E: invalid message to zyre actor src=" << zframe_strdup(src) << " op=" << zframe_strdup(op));
          }
          zmsg_destroy (&msg);
        }
        else if(which == zyre_socket(node))
        {
          zmsg_t *msg = zmsg_recv(which);
          zframe_t *event = zmsg_first(msg);
          if(zframe_streq(event, SHOUT) || zframe_streq(event, WHISPER))
          {
            //MLOG_GREEN(el::Level::Info, "zframe_streq(event, SHOUT) || zframe_streq(event, WHISPER) == true");
            for(const auto& s : scripts)
            {
              zmsg_t *m = zmsg_dup(msg);
              zframe_t *n = zframe_from(s.c_str());
              zmsg_prepend(m, &n);
              zmsg_send(&m, inproc);
            }
          }
          zmsg_destroy(&msg);
        }
      }
      catch(const std::exception& e)
      {
        MLOG_RED(el::Level::Error, e.what());
      }
      catch(...)
      {
        MLOG_RED(el::Level::Error, "unqnow");
      }
    }

    zpoller_destroy(&poller);
    zyre_stop (node);
    zclock_sleep (100);
    zyre_destroy (&node);
    zsock_destroy(&inproc);
  }

  zcert_t *server::pin_to_cert(const std::string& pin)
  {
    uint8_t salt[crypto_pwhash_scryptsalsa208sha256_SALTBYTES] = {
      128,165, 28,003,132,201,031,250,142,184,186,024, 8,167,68,075,
      053,231,105,160,230,167,144,201,176,158, 68,162,78,128,68,109
    };

    uint8_t seed[crypto_box_SEEDBYTES];
    
    if(0 != ::crypto_pwhash_scryptsalsa208sha256(
      seed, crypto_box_SEEDBYTES, pin.c_str(), pin.length(), salt,
      ::crypto_pwhash_scryptsalsa208sha256_opslimit_interactive(),
      ::crypto_pwhash_scryptsalsa208sha256_memlimit_interactive()))
    {
      MLOG_RED(el::Level::Error, "Could not create seed");
      return nullptr;
    }
    
    uint8_t pk[crypto_box_PUBLICKEYBYTES];
    uint8_t sk[crypto_box_SECRETKEYBYTES];
    
    if(0 != ::crypto_box_seed_keypair(pk, sk, seed))
    {
      MLOG_RED(el::Level::Error, "Could not create certificate");
      return nullptr;
    }
    
    return zcert_new_from(pk, sk);
  }


  std::atomic<uint64_t> client::cookie_n_;

  client::client(
          boost::asio::io_service& ios,
          const std::string& name,
          const server& srv)
    : ios_(ios)
    , name_(name)
    , dev_(srv.name())
    , zyre_sock_(zmq_socket(server::ctx(), ZMQ_DEALER))
    , zyre_(ios, zyre_sock_)
    , reply_timeout_(3) // 3 sec
  {
    zyre_.set_option(azmq::socket::identity(name_.data(), name_.size()));
    zyre_.connect((inproc_ZYRE "-" + dev_).c_str());
  }

  void client::s_join(const std::string& name)
  {
    if(ios_.stopped())
      return;
    std::array<boost::asio::const_buffer, 2> data = {{
      boost::asio::buffer(S_JOIN, sizeof(S_JOIN)-1),
      boost::asio::buffer(name.data(), name.size())
    }};
    zyre_.send(data);
  }

  void client::s_leave(const std::string& name)
  {
    if(ios_.stopped())
      return;
    std::array<boost::asio::const_buffer, 2> data = {{
      boost::asio::buffer(S_LEAVE, sizeof(S_LEAVE)-1),
      boost::asio::buffer(name.data(), name.size())
    }};
    zyre_.send(data);
  }

  void client::join(const std::string& name)
  {
    if(ios_.stopped())
      return;
    for(const auto& g : groups_)
      if(g == name) return;
    std::array<boost::asio::const_buffer, 2> data = {{
      boost::asio::buffer(JOIN, sizeof(JOIN)-1),
      boost::asio::buffer(name.data(), name.size())
    }};
    zyre_.send(data);
    groups_.push_back(name);
  }

  void client::leave(const std::string& name)
  {
    if(ios_.stopped())
      return;
    for(auto it=groups_.begin(); it<groups_.end(); it++)
      if(*it == name)
      {
        std::array<boost::asio::const_buffer, 2> data = {{
          boost::asio::buffer(LEAVE, sizeof(LEAVE)-1),
          boost::asio::buffer(name.data(), name.size())
        }};
        zyre_.send(data);
        groups_.erase(it);
      }
  }

#define ERROR_SHORT_EVENT "error short event"

  void client::start()
  {
    zyre_.async_receive(
      [&](boost::system::error_code const& e, azmq::message& msg, size_t bc)
      {
        if(!ios_.stopped())
        {
          try
          {
            if(e)
              throw std::runtime_error(e.message());
          
            if(!msg.more() || bc == 0)
              throw std::runtime_error(ERROR_SHORT_EVENT);
          
            azmq::message_vector vec;
            boost::system::error_code ec;
            zyre_.receive_more(vec, 0, ec);
            if(ec)
              throw std::runtime_error(ec.message());
          
            bool is_shout = false;
            bool is_whisper = false;
            bool is_local = false;
          
            std::string op = msg.string();
            /*std::string v0 = vec[0].string();
            std::string v1 = vec[1].string();
            std::string v2 = vec[2].string();
            std::string v3 = vec[3].string();
            std::string v4 = vec[4].string();
            std::string v5 = vec[5].string();

            ZLOG(ERROR) << (std::string() +
              + "op=" + op + "\n"
              + "v0=" + v0 + "\n"
              + "v1=" + v1 + "\n"
              + "v2=" + v2 + "\n"
              + "v3=" + v3 + "\n"
              + "v4=" + v4 + "\n"
              + "v5=" + v5 + "\n");*/

            if(op == SHOUT)
              is_shout = true;
          
            if(op == inproc_SHOUT)
              { is_shout = true; is_local = true; }
          
            if(op == WHISPER)
              is_whisper = true;
          
            if(op == inproc_WHISPER)
              { is_whisper = true; is_local = true; }
          
            size_t  peer = size_t(-1),
                    name = size_t(-1),
                    group = size_t(-1),
                    src = size_t(-1),
                    cmd = size_t(-1),
                    data = size_t(-1),
                    n = 0;
          
            if(!is_local)
            {
              peer = n; n++;
              name = n; n++;
          
              if(is_shout) {
                group = n; n++;
              }

              src = n; n++;
            }
            else
            {
              peer = n; n++;
            }
          
            if (is_shout || is_whisper)
            {
              if(n+5 > vec.size())
                throw std::runtime_error(ERROR_SHORT_EVENT);
              uint64_t ver_major = 0;
              uint64_t ver_minor = 0;
              msgpack::unpack(static_cast<const char *>(vec[n].data()), vec[n].size())
                .get().convert(ver_major); n++;
              msgpack::unpack(static_cast<const char *>(vec[n].data()), vec[n].size())
                .get().convert(ver_minor); n++;
              uint64_t cookie = 0;
              msgpack::unpack(static_cast<const char *>(vec[n].data()), vec[n].size())
                .get().convert(cookie); n++;
              cmd = n; n++;
              data = n; n++;

              /*MLOG_RED(el::Level::Warning, (std::string("CLIENT ") + name_ + " RECEIVED\n"
                        + "op=" + op + "\n"
                        + "peer=" + vec[peer].string() + "\n"
                        + "src=" + (src == size_t(-1) ? "" : vec[src].string()) + "\n"
                        + "cmd=" + vec[cmd].string() + "\n"
                        + "ver_major=" + std::to_string(ver_major) + "\n"
                        + "ver_minor=" + std::to_string(ver_minor) + "\n"
                        + "cookie=" + std::to_string(cookie)));*/

              if(ver_minor == VER_MINOR && ver_major == VER_MAJOR)
              {
                std::string cmd_s = vec[cmd].string();
                if(cmd_s == REPLY)
                {
                  const auto& ri = reply_.find(cookie);
                  if(ri != reply_.end())
                    ri->second.f(vec[data].string());
                }
                else
                {
                  const auto& f1 = meth_r_.find(cmd_s);
                  if(f1 != meth_r_.end())
                  {
                    std::string dd = vec[data].string();
                    f1->second(vec[data].string(), [&](const std::string& res){
                      do_send(
                         is_local ? inproc_WHISPER : WHISPER,
                         is_local ? sizeof(inproc_WHISPER)-1 : sizeof(WHISPER)-1,
                         static_cast<const char*>(vec[peer].data()), vec[peer].size(),
                         REPLY, sizeof(REPLY)-1,
                         res.data(), res.size(),
                         cookie);
                    });
                  }
                  else
                  {
                    const auto& f2 = meth_.find(cmd_s);
                    if(f2 != meth_.end())
                      f2->second(vec[data].string());
                  }
                }
              }
              else
              {
                MLOG_RED(el::Level::Error, (std::string("CLIENT ") + name_ + " RECEIVED\n"
                  + "Error version: name: " + (name == size_t(-1) ? "" : vec[name].string()) + "\n"
                  + "peer: " + (peer == size_t(-1) ? "" : vec[peer].string()) + "\n"
                  + "group: " + (group == size_t(-1) ? "" : vec[group].string()) + "\n"
                  + "src: " + (src == size_t(-1) ? "" : vec[src].string()) + "\n"
                  + "cmd: " + (cmd == size_t(-1) ? "" : vec[cmd].string()) + "\n"
                  + "ver_major: " + std::to_string(ver_major) + "\n"
                  + "ver_minor: " + std::to_string(ver_minor) + "\n"
                  + "cookie: " + std::to_string(cookie) + "\n"
                  + "cmd: " + (cmd == size_t(-1) ? "" : vec[cmd].string())));
                return;
              }
            }
            // remove timeout replyes
            std::vector<std::map<uint64_t, r_info_t>::iterator> r_timeout;
            for(auto it=reply_.begin(); it!=reply_.end(); it++)
              if(it->second.t < time(nullptr))
                r_timeout.push_back(it);
            for(auto it : r_timeout)
              reply_.erase(it);
          }
          catch(const std::exception& e)
          {
            MLOG_RED(el::Level::Error, e.what());
          }
          catch(...)
          {
            MLOG_RED(el::Level::Error, "unqnow");
          }
          
          start();
        }
      });
  }

  void client::stop()
  {
    if(zyre_sock_) {
      zmq_close(zyre_sock_);
      zyre_sock_ = nullptr;
    }
  }

  uint64_t client::do_send(
    const char *zyre_action_p, size_t zyre_action_sz,
    const char *peer_or_group_p, size_t peer_or_group_sz,
    const char *cmd_p, size_t cmd_sz,
    const char *params_p, size_t params_sz,
    uint64_t cookie)
  {
    if(ios_.stopped())
      return cookie;
    std::stringstream ss_v1;
    msgpack::pack(ss_v1, uint64_t(VER_MAJOR));
    std::string v1 = ss_v1.str();
    std::stringstream ss_v2;
    msgpack::pack(ss_v2, uint64_t(VER_MINOR));
    std::string v2 = ss_v2.str();
    if(cookie == COOKIE_NEXT) {
      cookie_n_++; cookie = cookie_n_;
    }
    std::stringstream ss_cookie;
    msgpack::pack(ss_cookie, cookie);
    std::string cookie_s = ss_cookie.str();

    std::array<boost::asio::const_buffer, 7> buf = {{
      boost::asio::buffer(zyre_action_p, zyre_action_sz),
      boost::asio::buffer(peer_or_group_p, peer_or_group_sz),
      boost::asio::buffer(v1.data(), v1.size()),
      boost::asio::buffer(v2.data(), v2.size()),
      boost::asio::buffer(cookie_s.data(), cookie_s.size()),
      boost::asio::buffer(cmd_p, cmd_sz),
      boost::asio::buffer(params_p, params_sz)
    }};

    boost::system::error_code ec;
    zyre_.send(buf, 0, ec);
    if(ec)
      throw std::runtime_error(ec.message());

    return cookie;
  }

  void client::do_send(const std::string& zyre_action, const std::string& group, const std::string& cmd, const std::string& pars)
  {
    do_send(
      zyre_action.data(), zyre_action.size(),
      group.data(), group.size(),
      cmd.data(), cmd.size(),
      pars.data(), pars.size(), COOKIE_NEXT);
  }

  void client::do_send_r(const std::string& zyre_action, const std::string& group, const std::string& cmd, const func_t& r, const std::string& pars)
  {
    uint64_t cookie = do_send(
      zyre_action.data(), zyre_action.size(),
      group.data(), group.size(),
      cmd.data(), cmd.size(),
      pars.data(), pars.size(), COOKIE_NEXT);
    reply_[cookie] = { time(nullptr) + reply_timeout_, r };
  }

  void client::do_send(const char *zyre_action, const char *group, char *cmd, const char *pars_p, size_t pars_sz)
  {
    do_send(
      zyre_action, strlen(zyre_action),
      group, strlen(group),
      cmd, strlen(cmd),
      pars_p, pars_sz, COOKIE_NEXT);
  }

  void client::do_send_r(const char *zyre_action, const char *group, char *cmd, const func_t& r, const char *pars_p, size_t pars_sz)
  {
    uint64_t cookie = do_send(
      zyre_action, strlen(zyre_action),
      group, strlen(group),
      cmd, strlen(cmd),
      pars_p, pars_sz, COOKIE_NEXT);
    reply_[cookie] = { time(nullptr) + reply_timeout_, r };
  }

  void client::on(const std::string& cmd, const func_t& f)
  {
    meth_[cmd] = f;
  }

  void client::on_r(const std::string& cmd, const func_r_t& f)
  {
    meth_r_[cmd] = f;
  }

}
