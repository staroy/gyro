#include "lthread.h"

#include <boost/filesystem/path.hpp>

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "zyre.lthread"

namespace lzyre
{
  std::string module_name(const std::string& file)
  {
    std::string name;

    boost::filesystem::path path(file);
    if(path.extension() == ".lua")
      path.replace_extension();

    for(auto e : path)
    {
      if(!name.empty())
          name += "_";
      name += e.string();
    }
    return name;
  }

  lthread_t::lthread_t(
    boost::asio::io_service& ios,
    const std::map<std::string, std::map<std::string, std::shared_ptr<zyre::server>>>& files)
    : ios_(ios)
    , strand_(ios)
    , files_(files)
  {
  }

  void lthread_t::run()
  {
    std::vector<sol::state> LL;
    std::map<std::string, std::map<std::string, std::shared_ptr<zyre::client>>> zc;

    for(auto f : files_)
    {
      LL.resize(LL.size()+1);
      auto& file = f.first;

      sol::state& L = LL.back();
      L.open_libraries();
      lldb::ldb::reg(L);
      timeout::reg(ios_, L);
      lclient::reg(L);
      
      std::string name = module_name(file);
      
      for(auto& s : f.second) // servers
      {
        std::shared_ptr<zyre::client> c = std::make_shared<zyre::client>(ios_, name, *s.second);
        c->start();
        c->s_join(name);
        zc[name][s.first] = c;
        L[s.first] = std::make_shared<lclient>(*c, s.second->groups());
      }

      //std::cout << "START: " << file << "\n"; 
      L.do_file(file);
    }

    try
    {
      for(;!ios_.stopped();)
        ios_.run();
    }
    catch (std::exception const & e)
    {
      MLOG_RED(el::Level::Error, e.what());
    }

    for(auto& n : zc)
    {
      for(auto& c : n.second)
      {
        c.second->s_leave(n.first);
        c.second->stop();
      }
    }
  }

  void lthread_t::start()
  {
    T_ = std::thread(std::bind(&lthread_t::run, this));
  }

  void lthread_t::join()
  {
    T_.join();
  }
}
