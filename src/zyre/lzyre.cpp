#include "lzyre.hpp"
#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "lzyre"

namespace sol { namespace stack {
  // arguments pusher
  int push(lua_State *L, lzyre::sol_mp_slice *p)
  {
    if(p->p && p->sz>0)
        return mp_unpack(L, p->p, p->sz);
    lua_pushnil(L);
    return 1;
  }
}}

namespace lzyre {

  class caller : public std::enable_shared_from_this<caller>
  {
    zyre::client& zyre_;
    std::string name_;
    std::vector<std::string> groups_;

  public:
    caller(zyre::client& zyre, const std::string& name, const std::vector<std::string>& groups)
      : zyre_(zyre)
      , name_(name)
      , groups_(groups)
    {}

    void on(sol::variadic_args args)
    {
      lua_State *L = args.lua_state();
      int limit = args.top();

      class callback_t : public std::enable_shared_from_this<callback_t>
      {
        bool m_;
        sol::table t_;
        sol::protected_function f_;
      public:
        callback_t(const sol::protected_function& f)
            : m_(false), f_(f)
        {}
        callback_t(const sol::protected_function& f, const sol::stack_object& t)
            : m_(true), f_(f), t_(t.as<sol::table>())
        {}
        void on(const std::string& pars)
        {
          sol_mp_slice s{pars.data(), pars.size()};
          if(m_)
          {
            sol::protected_function_result res = f_(t_, s);
            if (!res.valid())
            {
              sol::error err = res;
              MLOG_RED(el::Level::Error, err.what());
            }
          }
          else
          {
            sol::protected_function_result res = f_(s);
            if (!res.valid())
            {
              sol::error err = res;
              MLOG_RED(el::Level::Error, err.what());
            }
          }
        }
      };

      zyre::func_t r;
      if(lua_type(L, limit) == LUA_TFUNCTION)
      {
        std::shared_ptr<callback_t> callback(
          new callback_t(sol::stack_object(L, limit).as<sol::protected_function>())
        );
        r = std::bind(&callback_t::on, callback, std::placeholders::_1);
        limit--;
      }
      else if(lua_type(L, limit) == LUA_TTABLE && lua_type(L, limit-1) == LUA_TFUNCTION)
      {
        std::shared_ptr<callback_t> callback(
          new callback_t(
            sol::stack_object(L, limit-1).as<sol::protected_function>(),
            sol::stack_object(L, limit)
          )
        );
        r = std::bind(&callback_t::on, callback, std::placeholders::_1);
        limit-=2;
      }

      std::string pars;
      mp_pack(L, args.stack_index() + 1, limit, pars);
      for(const auto& g : groups_)
      {
        if(g[0] == '@')
          zyre_.do_send_r(inproc_WHISPER, g.data() + 1, name_, r, pars);
        else
          zyre_.do_send_r(SHOUT, g, name_, r, pars);
      }
    }
  };

  lclient::lclient(zyre::client& zyre, const std::vector<std::string>& groups)
    : zyre_(zyre)
    , groups_(groups)
  {
    for(const auto& g : groups_)
      zyre_.join(g);
  }

  lclient::~lclient()
  {
    for(const auto& g : groups_)
      zyre_.leave(g);
  }

  void lclient::join(sol::variadic_args args)
  {
    lua_State *L = args.lua_state();
    int limit = args.top();
    for(int n=args.stack_index(); n<=limit; n++)
    {
      std::string name = sol::stack_object(L, n).as<std::string>();
      bool exist = false;
      for(const auto& g : groups_)
        if(g == name)
          { exist = true; break; }
      if(!exist)
      {
        groups_.push_back(name);
        zyre_.join(name);
      }
    }
  }
  
  void lclient::leave(sol::variadic_args args)
  {
    lua_State *L = args.lua_state();
    int limit = args.top();
    for(int n=args.stack_index(); n<=limit; n++)
    {
      std::string name = sol::stack_object(L, n).as<std::string>();
      for(auto g=groups_.begin(); g!=groups_.end(); g++)
        if(*g == name) {
          groups_.erase(g);
          zyre_.leave(*g);
          break;
        }
    }
  }

  sol::object lclient::at(sol::variadic_args args)
  {
    std::vector<std::string> groups;
    lua_State *L = args.lua_state();
    int limit = args.top();
    for(int n=args.stack_index(); n<=limit; n++)
    {
      bool exist = false;    
      std::string name = sol::stack_object(L, n).as<std::string>();
      if(name[0] != '@') {
        for(const auto& g : groups_)
          if(g == name) { exist = true; break; }
      } else
        exist = true;
      if(exist)
        groups.push_back(name);
    }
    return sol::object(L, sol::in_place, std::make_shared<lclient>(zyre_, groups));
  }

  std::vector<std::string> lclient::groups()
  {
    return groups_;
  }

  void lclient::setter(sol::stack_object k, sol::stack_object v, sol::this_state L)
  {
    if(v.get_type() == sol::type::function) {
      sol::protected_function f = v.as<sol::protected_function>();
      zyre_.on_r(k.as<std::string>(), [this,f](const std::string& pars, zyre::func_t r){
        sol_mp_slice s{pars.data(), pars.size()};
        sol::protected_function_result res = f(shared_from_this(), s);
        if (res.valid()) {
          if(res.return_count()>0) {
            std::string buf;
            mp_pack(res.lua_state(),
              res.stack_index(),
              res.stack_index()+res.return_count()-1, buf);
            r(buf);
          }
        } else {
          sol::error err = res;
          MLOG_RED(el::Level::Error, err.what());
        }
      });
    }
  }

  sol::object lclient::getter(sol::stack_object k, sol::this_state L)
  {
    return sol::object(L, sol::in_place, std::make_shared<caller>(zyre_, k.as<std::string>(), groups_));
  }

  bool lclient::reg(sol::state_view& lua)
  {
    lua.new_usertype<caller>(
      "caller_type", sol::no_constructor,
      sol::meta_function::call, &caller::on);

    lua.new_usertype<lclient>(
      "zyre_type", sol::no_constructor,
      sol::meta_function::new_index, &lclient::setter,
      sol::meta_function::index, &lclient::getter,
      "join", &lclient::join,
      "leave", &lclient::leave,
      "at", &lclient::at,
      "groups", &lclient::groups);
    return true;
  }

}
