#include "zyre.hpp"
#include "lsrv.h"
#include "certs.h"
#include "auth.h"
#include "http.h"
#include "lzyre.hpp"
#include "ldb.h"
#include "timeout.h"
#include "lthread.h"

#include <iostream>
#include <thread>

#include <boost/program_options.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/algorithm/string.hpp>

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "zyre.lsrv"

namespace po = boost::program_options;

namespace lzyre {

lsrv::lsrv(boost::asio::io_service& ios)
  : ios_(ios), auth_(nullptr)
{
}

lsrv::~lsrv()
{
  zactor_destroy(&auth_);
}

void lsrv::start(int argc, char *argv[])
{
  uint16_t http_port = 0;
  bool verbose = false;
  std::string  name
             , data_path
             , log_file
             , log_level
             , config_file
             , http_host
             , http_zyre
             , docroot;
  std::vector<std::string> files;
  
  po::options_description desc("options");
  desc.add_options()
    (  "help,h"                                                            ,"help"                               )
    (  "name"         ,po::value(&name)                                    ,"name"                               )
    (  "data-path,d"  ,po::value(&data_path)->default_value(".")           ,"database path"                      )
    (  "log-file,l"   ,po::value(&log_file)                                ,"log file with path"                 )
    (  "log-level"    ,po::value(&log_level)                               ,"0-4 or categories"                  )
    (  "verbose,v"    ,po::value(&verbose)->default_value(false)           ,"zyre verbose"                       )
    (  "config-file,c",po::value(&config_file)                             ,"config file with path"              )
    (  "http-host"    ,po::value(&http_host)->default_value("127.0.0.1")   ,"http server host (0.0.0.0) listen"  )
    (  "http-port"    ,po::value(&http_port)->default_value(0)             ,"http server port listen"            )
    (  "http-zyre"    ,po::value(&http_zyre)->default_value("zyre")        ,"http server zyre lua name"          )
    (  "http-docroot" ,po::value(&docroot)->default_value(".")             ,"http server docroot path"           )
    (  "znet*"        ,po::value<std::string>()->multitoken()              ,"zyre net info"                      )
    (  "pin.*"        ,po::value<std::string>()                            ,"zyre net pins"                      )
    (  "lua*"         ,po::value<std::string>()->multitoken()              ,"zyre lua scripts"                   )
    (  "group.*"      ,po::value<std::string>()->multitoken()              ,"list of groups to join"             )
    (  "files"        ,po::value(&files)->multitoken()                     ,"list of lua script to start"        );
  
  po::positional_options_description pos_desc;
  pos_desc.add("files", -1);
  
  po::command_line_parser parser{argc, argv};
  parser.options(desc).positional(pos_desc).allow_unregistered();
  po::parsed_options parsed_options = parser.run();
  
  po::variables_map vm;
  po::store(parsed_options, vm);
  po::notify(vm);
  
  if(vm.count("help"))
  {
    std::cout << desc;
    return;
  }

  if(!config_file.empty())
  {
    std::ifstream fc(config_file);
    po::store(po::parse_config_file<char>(fc, desc), vm);
  }

  if(!log_file.empty()) mlog_configure(log_file, true);
  if(!log_level.empty()) mlog_set_log(log_level.c_str());
  
  lldb::ldb::data_path_ = data_path;
  
  std::map<std::string,       // prefix
    std::map<std::string,     // part name [<prefix><part name>]
      std::map<std::string,   // property name
        std::vector<std::string>>>> // multitoken value v1,v2,v3,...
          tree;

  // list of prefix
  const std::vector<std::string> vpfx = {"pin.","lua","group.","znet"};

  for(const po::variables_map::value_type& pair : vm)
  {
    for(const auto& pfx : vpfx)
    {
      const std::size_t pfx_size = pfx.size();
      if(pair.first.substr(0, pfx_size) != pfx)
        continue;

      const std::string& key =
        (pair.first[pfx_size] == '-') ? pair.first.substr(pfx_size + 1) : pair.first.substr(pfx_size);

      const auto& val = pair.second.as<std::string>();
      if(val.empty())
        continue;

      if(key.size()>0)
      {
        std::vector<std::string> vkey;
        boost::split(vkey, key, boost::is_any_of("."));
        if(vkey.size()>1)
        {
          const std::string& part = vkey[0].size() == 0 ? "*" : vkey[0];
          std::string prop = vkey[1];
          for(size_t n=2; n<vkey.size(); n++)
            prop += "." + vkey[n];
          boost::split(
            tree[pfx][part][prop], val, boost::is_any_of(",;"));
        }
        else
          boost::split(tree[pfx]["*"][key],val,boost::is_any_of(",;"));
      }
      else
        boost::split(tree[pfx]["*"]["*"],val,boost::is_any_of(",;"));
    }
  }

  /*for(auto& t1 : tree)
    for(auto& t2 : t1.second)
      for(auto& t3 : t2.second)
  {
    std::cout << "tree[" << t1.first << "][" << t2.first << "][" << t3.first << "]={";
    for(size_t v=0; v < t3.second.size(); v++)
    {
      if(v > 0)
        std::cout << ",";
      std::cout << t3.second[v];
    }
    std::cout << "}\n";
  }*/
  
  std::map<std::string, std::map<std::string, std::vector<std::string>>>& zpins = tree["pin."];
  std::map<std::string, std::map<std::string, std::vector<std::string>>>& zgroups = tree["group."];
  std::map<std::string, std::map<std::string, std::vector<std::string>>>& znets = tree["znet"];
  std::map<std::string, std::map<std::string, std::vector<std::string>>>& zluas = tree["lua"];

  std::map<std::string, std::string> pin_of_znets;
  std::map<std::string, std::set<std::string>> groups_of_vars;
  std::map<std::string, std::set<std::string>> vars_of_znets;
  std::map<std::string, std::set<std::string>> znets_of_vars;
  std::map<std::string, std::set<std::string>> vars_of_luas;
  std::map<std::string, std::set<std::string>> lthr_of_luas;

  for(const auto& t : zluas)
  {
    auto& ll = lthr_of_luas[t.first];
    for(const auto& g : t.second)
      for(const auto& l : g.second)
        ll.insert(l);
  }

  auto& ml = lthr_of_luas["*"];
  for(auto& f : files)
    ml.insert(f);

  for(const auto& m : znets)
    for(const auto& p : m.second)
  {
    if(p.first == "alias" || p.first == "*")
    {
      auto& vv = vars_of_znets[m.first];
      for(auto& v : p.second) {
        vv.insert(v);
        znets_of_vars[v].insert(m.first);
      }
    }
  }
  
  for(const auto& m : znets)
    for(const auto& p : m.second)
  {
    if(p.first == "lua")
    {
      for(const auto& l : p.second)
      {
        if(l[0] == '@')
        {
          if(l.size()==1)
          {
            // assign from all file list arguments without switched
            for(auto& f : files) {
              auto& vv = vars_of_luas[f];
              for(auto& v : vars_of_znets[m.first])
                vv.insert(v);
            }
          }
          else if(isdigit(l[1]))
          {
            // assign from n indexed file argument without switched
            size_t n = size_t(atoi(l.substr(1).c_str()));
            n = (n == 0 ? files.size() : n - 1);
            std::string f = n<files.size() ? files[n] : l;
            auto& vv = vars_of_luas[f];
            for(auto& v : vars_of_znets[m.first])
              vv.insert(v);
          }
          else
          {
            // assign from zlua... named list of lua files
            std::vector<std::string> ref;
            std::string lr = l.substr(1);
            boost::split(ref, lr, boost::is_any_of("."));
            if(ref.size()==1)
            {
              for(auto& t : zluas)
                for(auto& f : t.second[ref[0]]) {
                  auto& vv = vars_of_luas[f];
                  for(auto& v : vars_of_znets[m.first])
                    vv.insert(v);
                }
            }
            else
            {
              size_t n = size_t(atoi(ref[1].c_str()));
              for(auto& t : zluas)
              {
                auto& la = t.second[ref[0]];
                n = (n == 0 ? la.size() : n - 1);
                std::string f = n<la.size() ? la[n] : l;
                auto& vv = vars_of_luas[f];
                for(auto& v : vars_of_znets[m.first])
                  vv.insert(v);
              }
            }
          }
        }
        else
        {
          auto& vv = vars_of_luas[l];
          for(auto& v : vars_of_znets[m.first])
            vv.insert(v);
          lthr_of_luas["*"].insert(l);
        }
      }
    }
    else if(p.first == "pin")
    {
      auto& pin = pin_of_znets[m.first];
      auto& v = p.second[0];
      if(v[0] == '@')
      {
        if(isdigit(v[1]))
        {
          auto& vpin = zpins["*"]["*"];
          size_t n = size_t(atoi(v.substr(1).c_str()));
          n = (n == 0 ? vpin.size() : n - 1);
          pin = n<vpin.size() ? vpin[n] : v;
        }
        else
        {
          // assign from zpins... named list of pin
          std::vector<std::string> ref;
          std::string pr = v.substr(1);
          boost::split(ref, pr, boost::is_any_of("."));
          if(ref.size()>1){
            auto& vpin = zpins["*"][ref[0]];
            size_t n = size_t(atoi(ref[1].c_str()));
            n = (n == 0 ? vpin.size() : n - 1);
            pin = n<vpin.size() ? vpin[n] : v;
          }
          else
            std::cout << "error multiply pins\n";
        }
      }
      else
        pin = v;
    }
    else if(p.first == "group")
    {
      for(const auto& g : p.second)
      {
        if(g[0] == '@')
        {
          if(isdigit(g[1]))
          {
            const auto& gg = zgroups["*"]["*"];
            size_t n = size_t(atoi(g.substr(1).c_str()));
            n = (n == 0 ? gg.size() : n - 1);
            std::string G = n < gg.size() ? gg[n] : g;
            for(auto& v : vars_of_znets[m.first])
              groups_of_vars[v].insert(G);
          }
          else
          {
            // assign from zlua... named list of lua files
            std::vector<std::string> ref;
            std::string gn = g.substr(1);
            boost::split(ref, gn, boost::is_any_of("."));
            if(ref.size() == 1)
            {
              for(auto& G : zgroups["*"][ref[0]])
                for(auto& v : vars_of_znets[m.first])
                  groups_of_vars[v].insert(G);
            }
            else
            {
              auto& gg = zgroups["*"][ref[0]];
              size_t n = size_t(atoi(ref[1].c_str()));
              n = (n == 0 ? gg.size() : n - 1);
              std::string G = n<gg.size() ? gg[n] : g;
              for(auto& v : vars_of_znets[m.first])
                groups_of_vars[v].insert(G);
            }
          }
        }
        else
        {
          for(auto& v : vars_of_znets[m.first])
            groups_of_vars[v].insert(g);
        }
      }
    }
  }
  
  certs_t *certs = certs_new();
  
  for(auto& it : znets_of_vars)
  {
    std::string n0;
    for(auto& n : it.second)
    {
      std::string pin = n + pin_of_znets[n];
      zcert_t *crt = zyre::server::pin_to_cert(pin);
      certs_insert(certs, ("zap-" + it.first).c_str(), &crt);
      if(n0.empty()) n0 = pin;
    }
    
    std::vector<std::string> gg;
    for(auto& g : groups_of_vars[it.first])
      gg.push_back(g);

    auto s_ptr = std::make_shared<zyre::server>(it.first, gg, n0);

    if(verbose)
      s_ptr->set_verbose();
  
    srvs_[it.first] = s_ptr;
  }
  
  auth_ = zactor_new(domain_auth, certs);
  
  for(auto s : srvs_) s.second->start();
  
  if(http_port && srvs_.find(http_zyre) != srvs_.end())
    http_.reset(new http::server(ios_, http_host, http_port, docroot, *srvs_[http_zyre]));
  
  zclock_sleep(100);

  /*for(auto& t : lthr_of_luas)
  {
    size_t n=0;
    std::cout << "thr[" << t.first << "]={";
    for(auto& l : t.second)
    {
      if(n > 0)
        std::cout << ",";
      std::cout << l;
      n++;
    }
    std::cout << "}\n";
  }*/
  
  for(const auto& t : lthr_of_luas)
  {
    std::map<std::string,
      std::map<std::string,
        std::shared_ptr<zyre::server>>> files;
    //std::cout << t.first << "{\n";
    for(const auto& f : t.second)
      for(auto& s : vars_of_luas[f])
      {
        files[f][s] = srvs_[s];
        //std::cout << "[" << f << "][" << s << "]\n";
      }
    //std::cout << "}\n";
    S_.emplace_back(ios_, files);
  }
  
  for(auto& s : S_)
    s.start();
  
  if(http_)
    http_->start();
}

void lsrv::stop()
{
  for(auto s : srvs_)
    s.second->stop();
}

void lsrv::join()
{
  for(auto& s : S_)
    s.join();
  
  if(http_)
    http_->join();
  
  for(auto& s : srvs_)
    s.second->join();
}

}