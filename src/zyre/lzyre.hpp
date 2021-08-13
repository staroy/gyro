#ifndef __zyre_ll_h__
#define __zyre_ll_h__

#include "lua.hpp"

#include <string>
#include <iostream>

namespace lzyre {

    struct sol_mp_slice { const char *p; size_t sz; };
    struct sol_mp_buf { std::string data; };

}

namespace sol {
    namespace stack {
        // arguments pusher
        int push(lua_State*, lzyre::sol_mp_slice *);
    }
}

#include "sol/sol.hpp"
#include "lua_cmsgpack.h"

namespace sol
{
    template <>
    struct lua_size<lzyre::sol_mp_buf> : std::integral_constant<int, 1> {};
    template <>
    struct lua_type_of<lzyre::sol_mp_buf> : std::integral_constant<sol::type, sol::type::poly> {};

    namespace stack
    {
        // return checker
        template <>
        struct unqualified_checker<lzyre::sol_mp_buf, type::poly> {
            template <typename Handler>
            static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
                return true;
            }
        };

        // return getter
        template <>
        struct unqualified_getter<lzyre::sol_mp_buf> {
            static lzyre::sol_mp_buf get(lua_State* L, int index, record& tracking) {
                int top = lua_gettop(L);
                lzyre::sol_mp_buf buf;
                if(top >= index)
                    mp_pack(L, index, top, buf.data);
                return buf;
            }
        };

        // return pusher
        template <>
        struct unqualified_pusher<lzyre::sol_mp_buf> {
            static int push(lua_State* L, const lzyre::sol_mp_buf& buf) {
                if(!buf.data.empty())
                    return mp_unpack(L, buf.data.data(), buf.data.size());
                lua_pushnil(L);
                return 1;
            }
        };
    }
}

#include "zyre.hpp"

namespace lzyre
{
  class lclient : public std::enable_shared_from_this<lclient>
  {
  friend class caller;
  protected:
    zyre::client& zyre_;
    std::vector<std::string> groups_;

  public:
    lclient(zyre::client& zyre, const std::vector<std::string>& groups);
    ~lclient();

    void join(sol::variadic_args args);
    void leave(sol::variadic_args args);

    sol::object at(sol::variadic_args args);
    std::vector<std::string> groups();

    void setter(sol::stack_object k, sol::stack_object v, sol::this_state L);
    sol::object getter(sol::stack_object k, sol::this_state L);
    static bool reg(sol::state_view& lua);
  };
}

#endif
