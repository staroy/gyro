#ifndef L_DATABASE_H
#define L_DATABASE_H

#include "leveldb/db.h"
#include "leveldb/iterator.h"
#include "leveldb/write_batch.h"

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <mutex>

#include <boost/program_options.hpp>

namespace lldb { 

class db;

class batch_t
{
    friend db;

    ::leveldb::WriteBatch             batch_;

protected:
    std::shared_ptr<::leveldb::DB>    db_;

public:
    batch_t(const std::shared_ptr<::leveldb::DB>&);
    ~batch_t();
    bool valid();
};

class db
{
    static std::map<
        std::string, std::weak_ptr<::leveldb::DB>
    >                                       g_db;
    static std::mutex                       g_mx;

    std::shared_ptr<batch_t>                batch_;
    std::unique_ptr<::leveldb::Iterator>      iterator_;

    bool __open();

protected:
    std::string                             path_;
    std::string                             prefix_;
    std::shared_ptr<::leveldb::DB>            db_;

public:
    bool open(const char *path);
    bool open(const char *path, const char *key, size_t key_sz);

    bool opened() { return db_.get() != nullptr; }

    bool attach(const db *p);
    bool attach(const db *p, const char *key, size_t key_sz);

    void set_prefix(const char *key, size_t key_sz);
    void add_prefix(const char *key, size_t key_sz);
    const char *get_prefix(size_t *sz);
    const char *get_path();

    bool __get(const char *key, size_t key_sz, std::string& res);
    void __put(const char *key, size_t key_sz, const char *val, size_t val_sz);
    bool __del(const char *key, size_t key_sz);

    bool get(const char *key, size_t key_sz, std::string& res);
    void put(const char *key, size_t key_sz, const char *val, size_t val_sz);
    bool del(const char *key, size_t key_sz);

    std::shared_ptr<batch_t> batch();
    void batch(const std::shared_ptr<batch_t>& b_ptr);
    void commit();

    bool skip(int step, const char *pfx, size_t pfx_sz, std::string& key, std::string& val, bool iterator_clear=false);
    bool seek(const char *pfx, size_t pfx_sz, std::string& key, std::string& val);

    inline bool get(std::string& res) { return get(0, 0, res); }
    inline void put(const char *val, size_t val_sz) { return put(0, 0, val, val_sz); }
    inline bool del() { return del(0, 0); }
    inline bool seek(std::string& key, std::string& val) { return seek(0, 0, key, val); }
    inline bool skip(int step, std::string& key, std::string& val) { return skip(step, 0, 0, key, val); }
    inline bool next(std::string& key, std::string& val) { return skip(1, 0, 0, key, val); }
    inline bool prev(std::string& key, std::string& val) { return skip(-1, 0, 0, key, val); }
    inline bool first(std::string& key, std::string& val) { return skip(1, 0, 0, key, val, true); }
    inline bool last(std::string& key, std::string& val) { return skip(-1, 0, 0, key, val, true); }
};

template<char L>
class db_layer : public db
{
public:
    db_layer()
    {
        prefix_.push_back(L);
    }
    bool open(const char *path)
    {
        if(db::open(path))
            return true;
        return false;
    }
    bool open(const char *path, const char *key, size_t key_sz)
    {
        if(db::open(path, key, key_sz))
            return true;
        return false;
    }
    bool attach(const db *p)
    {
        if(db::attach(p))
            return true;
        return false;
    }
    bool attach(const db *p, const char *key, size_t key_sz)
    {
        if(db::attach(p, key, key_sz))
            return true;
        return false;
    }
    inline void set_prefix(const char *key, size_t key_sz)
    {
        prefix_.clear();
        prefix_.push_back(L);
        if(key && key_sz)
        {
            prefix_.insert(prefix_.end(), key, key+key_sz);
        }
    }
    inline const char *get_prefix(size_t *sz)
    {
        if(sz)
            *sz = prefix_.size()-1;
        return prefix_.data()+1;
    }
    operator const std::shared_ptr<::leveldb::DB>& () { return db_; }
};

}

char const DB_DATA_LAYER =  char(128);
char const DB_META_LAYER =  char(136);

#endif
