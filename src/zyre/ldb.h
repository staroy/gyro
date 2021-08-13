#ifndef L_DB_LUA_H
#define L_DB_LUA_H

#include "lua.hpp"
#include "db.h"

#include <string>

namespace lldb { 

    struct sol_mp_slice { const char *p; size_t sz; };
    struct sol_mp_buf { std::string data; };

}

namespace sol {
    namespace stack {
        // arguments pusher
        int push(lua_State*, lldb::sol_mp_slice*);
    }
}

#include "sol/sol.hpp"
#include "lua_cmsgpack.h"

namespace sol
{
    template <>
    struct lua_size<lldb::sol_mp_buf> : std::integral_constant<int, 1> {};
    template <>
    struct lua_type_of<lldb::sol_mp_buf> : std::integral_constant<sol::type, sol::type::poly> {};

    namespace stack
    {
        // return checker
        template <>
        struct unqualified_checker<lldb::sol_mp_buf, type::poly> {
            template <typename Handler>
            static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
                return true;
            }
        };

        // return getter
        template <>
        struct unqualified_getter<lldb::sol_mp_buf> {
            static lldb::sol_mp_buf get(lua_State* L, int index, record& tracking) {
                int top = lua_gettop(L);
                lldb::sol_mp_buf buf;
                if(top >= index)
                    mp_pack(L, index, top, buf.data);
                return buf;
            }
        };

        // return pusher
        template <>
        struct unqualified_pusher<lldb::sol_mp_buf> {
            static int push(lua_State* L, const lldb::sol_mp_buf& buf) {
                if(!buf.data.empty())
                    return mp_unpack(L, buf.data.data(), buf.data.size());
                lua_pushnil(L);
                return 1;
            }
        };
    }
}

namespace lldb { 

class ldb : public  std::enable_shared_from_this<ldb>
{
public:
    struct pairs_t {
        pairs_t(ldb *p, const char *key, size_t sz)
            : self(std::make_shared<ldb>(p, key, sz)) {}
        static int iterator(lua_State* L);
        std::shared_ptr<ldb> self;
    };
    struct rpairs_t {
        rpairs_t(ldb *p, const char *key, size_t sz)
            : self(std::make_shared<ldb>(p, key, sz)) {}
        static int iterator(lua_State* L);
        std::shared_ptr<ldb> self;
    };

private:
    db_layer<DB_DATA_LAYER> db_;
    std::string name_;
    size_t postfix_len_;
public:
    ldb(const std::string& name);
    ldb(const std::string& name, const char *pfx, size_t sz);
    ldb(const std::string& name, const std::string& pfx);

    ldb(ldb *self);
    ldb(ldb *self, const char *pfx, size_t sz);

    const std::string& get_name();

    sol::object getter(sol::stack_object key, sol::this_state L);
    void setter(sol::stack_object key, sol::stack_object value, sol::this_state);

    sol::object at(sol::variadic_args args);

    sol_mp_buf get(sol::variadic_args args);
    void put(sol::variadic_args args);
    void del(sol::variadic_args args);

    std::shared_ptr<batch_t> get_batch();
    void set_batch(std::shared_ptr<batch_t>);
    void commit();

    sol_mp_buf seek(sol::variadic_args args);
    sol_mp_buf skip(int n);
    sol_mp_buf first();
    sol_mp_buf last();
    sol_mp_buf next();
    sol_mp_buf prev();

    pairs_t pairs(sol::variadic_args args);
    rpairs_t rpairs(sol::variadic_args args);

    static std::string data_path_;
    static bool reg(sol::state_view& lua);
};

}
#endif
