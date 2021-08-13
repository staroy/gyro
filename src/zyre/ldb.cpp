#include "ldb.h"

#include <iostream>
#include <sstream>

#include "lua_cmsgpack.h"

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "leveldb.ldb"

/*extern "C"
{
  LUALIB_API int luaopen_lldb(lua_State *L)
  {
    sol::state_view lua = L;
    lldb::ldb::reg(lua);
    return 0;
  }
}*/

namespace sol { namespace stack {
  // arguments pusher
  int push(lua_State *L, lldb::sol_mp_slice *p)
  {
    if(p->p && p->sz>0)
        return mp_unpack(L, p->p, p->sz);
    lua_pushnil(L);
    return 1;
  }
}}

namespace lldb { 

std::string ldb::data_path_;

bool ldb::reg(sol::state_view& lua)
{
    auto db_new = [](const std::string& name) -> std::shared_ptr<ldb> {
        return std::make_shared<ldb>(name);
    };

    lua.new_usertype<batch_t>("batch",
        "new", sol::no_constructor,
        "valid", &batch_t::valid);

    lua.new_usertype<ldb>("database",
        sol::call_constructor, db_new, "new", db_new,
        sol::meta_function::index, &ldb::getter,
        sol::meta_function::new_index, &ldb::setter,
        "at", &ldb::at,
        "get", &ldb::get,
        "put", &ldb::put,
        "del", &ldb::del,
        "batch", sol::overload(&ldb::get_batch, &ldb::set_batch),
        "commit", &ldb::commit,
        "seek", &ldb::seek,
        "skip", &ldb::skip,
        "first", &ldb::first,
        "last", &ldb::last,
        "next", &ldb::next,
        "prev", &ldb::prev,
        "pairs", &ldb::pairs,
        "rpairs", &ldb::rpairs
    );

    return true;
}

ldb::ldb(const std::string& name)
    : name_(name)
    , postfix_len_(0)
{
    std::string path;
    if(data_path_.size()>0)
    {
      if(data_path_.back()=='/' || data_path_.back()=='\\')
        path = data_path_+name;
      else
        path = data_path_+"/"+name;
    }
    else
      path = name;
    db_.open(path.c_str());
}

ldb::ldb(const std::string& name, const char *pfx, size_t sz)
    : name_(name)
    , postfix_len_(sz)
{
    db_.open(name.c_str(), pfx, sz);
}

ldb::ldb(const std::string& name, const std::string& pfx)
    : name_(name)
    , postfix_len_(pfx.size())
{
    db_.open(name.c_str(), pfx.data(), pfx.size());
}

ldb::ldb(ldb *self)
    : name_(self->name_)
    , postfix_len_(0)
{
    db_.attach(&self->db_);
}

ldb::ldb(ldb *self, const char *pfx, size_t sz)
    : name_(self->name_)
    , postfix_len_(sz)
{
    db_.attach(&self->db_, pfx, sz);
}

const std::string& ldb::get_name()
{
    return name_;
}

sol::object ldb::at(sol::variadic_args args)
{
    lua_State *L = args.lua_state();

    std::string key;
    mp_pack(L, args.stack_index(), args.top(), key);

    return sol::object(L, sol::in_place, std::make_shared<ldb>(this, key.data(), key.size()));
}

sol::object ldb::getter(sol::stack_object key, sol::this_state L)
{
    std::string buf_key;
    mp_pack(L, key.stack_index(), key.stack_index(), buf_key);
    return sol::object(L, sol::in_place, std::make_shared<ldb>(this, buf_key.data(), buf_key.size()));
}

void ldb::setter(sol::stack_object k, sol::stack_object v, sol::this_state L)
{
    std::string key;
    mp_pack(L, k.stack_index(), k.stack_index(), key);

    switch(v.get_type())
    {
    case sol::type::lua_nil:
      db_.del(key.data(), key.size());
      break;
    default:
      {
        std::string val;
        mp_pack(L, v.stack_index(), v.stack_index(), val);
        db_.put(key.data(), key.size(), val.data(), val.size());
      }
      break;
    }
}

sol_mp_buf ldb::get(sol::variadic_args args)
{
    lua_State *L = args.lua_state();

    int limit = args.top();

    std::string key;
    mp_pack(L, args.stack_index(), limit, key);

    sol_mp_buf val;
    db_.get(key.data(), key.size(), val.data);

    return val;
}

void ldb::put(sol::variadic_args args)
{
    lua_State *L = args.lua_state();

    std::string key, val;
    mp_pack(L, args.stack_index(), args.top()-1, key);
    mp_pack(L, args.top(), args.top(), val);

    db_.put(key.data(), key.size(), val.data(), val.size());
}

void ldb::del(sol::variadic_args args)
{
    lua_State *L = args.lua_state();

    std::string key;
    mp_pack(L, args.stack_index(), args.top(), key);

    db_.del(key.data(), key.size());
}

std::shared_ptr<batch_t> ldb::get_batch()
{
    return db_.batch();
}

void ldb::set_batch(std::shared_ptr<batch_t> b)
{
    db_.batch(b);
}

void ldb::commit()
{
    db_.commit();
}

sol_mp_buf ldb::seek(sol::variadic_args args)
{
    lua_State *L = args.lua_state();

    std::string key;
    mp_pack(L, args.stack_index(), args.top(), key);

    sol_mp_buf res;
    db_.seek(key.data(), key.size(), res.data, res.data);

    return res;
}

sol_mp_buf ldb::skip(int n)
{
    sol_mp_buf res;
    db_.skip(n, res.data, res.data);
    return res;
}

sol_mp_buf ldb::first()
{
    sol_mp_buf res;
    db_.first(res.data, res.data);
    return res;
}

sol_mp_buf ldb::last()
{
    sol_mp_buf res;
    db_.last(res.data, res.data);
    return res;
}

sol_mp_buf ldb::next()
{
    sol_mp_buf res;
    db_.next(res.data, res.data);
    return res;
}

sol_mp_buf ldb::prev()
{
    sol_mp_buf res;
    db_.prev(res.data, res.data);
    return res;
}

int ldb::pairs_t::iterator(lua_State* L)
{
    sol::stack_object selfobj(L, 1);
    ldb& self = selfobj.as<ldb>();
    return sol::stack::push(L, self.next());
}

int ldb::rpairs_t::iterator(lua_State* L)
{
    sol::stack_object selfobj(L, 1);
    ldb& self = selfobj.as<ldb>();
    return sol::stack::push(L, self.prev());
}

ldb::pairs_t ldb::pairs(sol::variadic_args args)
{
    lua_State *L = args.lua_state();
    std::string key;
    mp_pack(L, args.stack_index(), args.top(), key);
    return pairs_t(this, key.data(), key.size());
}

ldb::rpairs_t ldb::rpairs(sol::variadic_args args)
{
    lua_State *L = args.lua_state();
    std::string key;
    mp_pack(L, args.stack_index(), args.top(), key);
    return rpairs_t(this, key.data(), key.size());
}

}
