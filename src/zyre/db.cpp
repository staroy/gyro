#include "db.h"

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/options.h"

#include <iostream>
#include <sstream>

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "leveldb.db"

namespace lldb { 

std::map<
  std::string, std::weak_ptr<::leveldb::DB >
>           db::g_db;
std::mutex  db::g_mx;

bool db::attach(const db *p)
{
  if(p && p->db_.get())
  {
  db_ = p->db_;
  path_ = p->path_;
  prefix_ = p->prefix_;
  return true;
  }
  return false;
}

bool db::attach(const db *p, const char *key, size_t key_sz)
{
  if(p && p->db_.get())
  {
  db_ = p->db_;
  path_ = p->path_;
  prefix_ = p->prefix_;
  add_prefix(key, key_sz);
  return true;
  }
  return false;
}

bool db::open(const char *path)
{
  path_ = path;
  return __open();
}

bool db::open(const char *path, const char *key, size_t key_sz)
{
  path_ = path;
  set_prefix(key, key_sz);
  return __open();
}

bool db::__open()
{
  if(path_.empty())
    return false;

  std::lock_guard<std::mutex> lock(g_mx);

  auto it = g_db.find(path_.c_str());

  if (it != g_db.end())
  {
    if (it->second.use_count() > 0)
    {
      db_ = it->second.lock();
      return true;
    }
    else
      g_db.erase(path_.c_str());
  }

  ::leveldb::Options opt;
  //opt.paranoid_checks = true;
  opt.create_if_missing = true;
  opt.paranoid_checks = true;
  opt.block_cache = ::leveldb::NewLRUCache(8 * 1048576);  // 8MB cache

  ::leveldb::DB *pDB = 0;
  if( !::leveldb::DB::Open(opt, path_.c_str(), &pDB).ok() )
  {
    ::leveldb::Status rc = ::leveldb::RepairDB(path_.c_str(), opt);
    if( rc.ok() )
    {
      MLOG_RED(el::Level::Error, "repaired db " << path_);
      rc = ::leveldb::DB::Open(opt, path_.c_str(), &pDB);
      if( !rc.ok() )
      {
        MLOG_RED(el::Level::Error, "error open db " << path_);
        return false;
      }
    }
    else
    {
      MLOG_RED(el::Level::Error, "error repair db " << path_);
      return false;
    }
  }

  db_.reset(pDB);
  g_db[path_] = db_;

  return true;
}

bool db::__get(const char *key, size_t key_sz, std::string& res)
{
  ::leveldb::Slice slice_key(key, key_sz);

  ::leveldb::ReadOptions op;
  ::leveldb::Status rc = db_->Get(op, slice_key, &res);

  if(!rc.ok())
    return false;

  return true;
}

bool db::get(const char *key, size_t key_sz, std::string& res)
{
  if(!db_.get())
    return false;

  std::vector<char> raw_key(prefix_.begin(), prefix_.end());
  if(key && key_sz)
    raw_key.insert(raw_key.end(), key, key+key_sz);

  //printf_hex("get key: %s", raw_key.data(), raw_key.size());
  return __get(raw_key.data(), raw_key.size(), res);
}

batch_t::batch_t(const std::shared_ptr<::leveldb::DB>& db)
  : db_(db)
{
}

batch_t::~batch_t()
{
  if(db_)
  {
    ::leveldb::WriteOptions op;
    ::leveldb::Status rc = db_->Write(op, &batch_);
    if(!rc.ok())
      MLOG_RED(el::Level::Error, "error commit");
  }
  else
    MLOG_RED(el::Level::Error, "error commit, db is nullptr");
}

bool batch_t::valid()
{
  return bool(db_);
}

std::shared_ptr<batch_t> db::batch()
{
  if(db_.get())
    return batch_;

  MLOG_RED(el::Level::Error, "error create batch, db is nullptr");
  return std::shared_ptr<batch_t>();
}

void db::batch(const std::shared_ptr<batch_t>& b_ptr)
{
  if(b_ptr->db_ == db_)
    batch_ = b_ptr;
  else
    MLOG_RED(el::Level::Error, "attached batch with other db object");
}

void db::commit()
{
  batch_.reset();
}

void db::__put(const char *key, size_t key_sz, const char *val, size_t val_sz)
{
  ::leveldb::Slice slice_key(key, key_sz);
  ::leveldb::Slice slice_val(val, val_sz);

  //XLOG(ERROR) << "DB: " << path_;
  //printf_hex("put key: %s\n", slice_key.data(), slice_key.size());

  if(!batch_)
  {
    ::leveldb::WriteOptions op;
    ::leveldb::Status rc = db_->Put(op, slice_key, slice_val);
    if(!rc.ok())
      MLOG_RED(el::Level::Error, "error put");
  }
  else
    batch_->batch_.Put(slice_key, slice_val);
}

void db::put(const char *key, size_t key_sz, const char *val, size_t val_sz)
{
  if(!db_.get())
    return;

  std::vector<char> raw_key(prefix_.begin(), prefix_.end());
  if(key && key_sz)
    raw_key.insert(raw_key.end(), key, key+key_sz);

  __put(raw_key.data(), raw_key.size(), val, val_sz);
}

bool db::__del(const char *key, size_t key_sz)
{
  ::leveldb::Slice slice_key(key, key_sz);

  if(!batch_.get())
  {
    ::leveldb::WriteOptions op;
    ::leveldb::Status rc = db_->Delete(op, slice_key);

    if(!rc.ok())
      return false;
  }
  else
    batch_->batch_.Delete(slice_key);

  return true;
}

bool db::del(const char *key, size_t key_sz)
{
  if(!db_.get())
    return false;

  std::vector<char> raw_key(prefix_.begin(), prefix_.end());
  if(key && key_sz)
    raw_key.insert(raw_key.end(), key, key+key_sz);

  return __del(raw_key.data(), raw_key.size());
}

bool db::skip(int step, const char *pfx, size_t pfx_sz, std::string& item_key, std::string& item_val, bool iterator_clear)
{
  if(!db_.get())
    return false;

  if(iterator_clear)
    iterator_.reset();

  if (!iterator_)
  {
    ::leveldb::ReadOptions op;
    iterator_.reset( db_->NewIterator(op) );

    std::vector<char> b;

    if(prefix_.size()>0)
      b.assign(prefix_.begin(), prefix_.end());

    if(pfx && (pfx_sz>0))
      b.insert(b.end(), pfx, pfx+pfx_sz);

    if(b.size()>0)
    {
      ::leveldb::Slice slice(b.data(), b.size());
      if(step<0)
      {
        for(auto n=b.rbegin();n<b.rend();n++){
          uint16_t c=*n; c++; *n=c;
          if(c<256) break;
        }
        iterator_->Seek(slice);
        if(iterator_->Valid())
          iterator_->Prev();
        else
          iterator_->SeekToLast();
      }
      else if(step>0)
        iterator_->Seek(slice);
    }
    else if(step>0)
      iterator_->SeekToFirst();
    else if(step<0)
      iterator_->SeekToLast();
  }
  else if(step>0)
    for(; step>0 && iterator_->Valid(); step--)
      iterator_->Next();
  else if(step<0)
    for(; step<0 && iterator_->Valid(); step++)
        iterator_->Prev();

  if(iterator_->Valid())
  {
    ::leveldb::Slice key = iterator_->key();
    if(key.size() >= prefix_.size())
    {
      if(prefix_.size()==0 || memcmp(key.data(), prefix_.data(), prefix_.size()) == 0)
      {
        ::leveldb::Slice val = iterator_->value();
        item_val.insert(item_val.end(), val.data(), val.data()+val.size());
        item_key.insert(item_key.end(), key.data()+prefix_.size(), key.data()+key.size());
        return true;
      }
      /*else
      {
        printf_hex("slice key: %s", key.data(), key.size());
        printf_hex("prefix: %s", prefix_.data(), prefix_.size());
      }*/
    }
  }
  iterator_.reset();
  return false;
}

bool db::seek(const char *pfx, size_t pfx_sz, std::string& key, std::string& val)
{
  if(!skip(1, pfx, pfx_sz, key, val, true) || key.size() < pfx_sz)
    return false;
  if(0 != memcmp(pfx, key.data(), pfx_sz))
    return false;
  return true;
}

void db::set_prefix(const char *key, size_t key_sz)
  { prefix_.clear(); if(key && key_sz) { prefix_.insert(prefix_.end(), key, key+key_sz); } }
void db::add_prefix(const char *key, size_t key_sz)
  { if(key && key_sz) { prefix_.insert(prefix_.end(), key, key+key_sz); } }
const char *db::get_prefix(size_t *sz)
  { if(sz) *sz = prefix_.size(); return prefix_.data(); }
const char *db::get_path()
  { return path_.c_str(); }

}
