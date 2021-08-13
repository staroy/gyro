#ifndef _LLDB_DB_HPP
#define _LLDB_DB_HPP

#include "db.h"
#include "iterator_tpl.h"

#include <msgpack.hpp>

namespace lldb { namespace cxx {

    class db
    {
        db_layer<DB_DATA_LAYER> db_;

        class stream
        {
            std::string& data_;
        public:
            stream(std::string& data) : data_(data) {}
            void write(const char* buf, size_t len) { data_.append(buf, buf+len); }
        };

        class writer
        {
            std::string& data_;

            void write(msgpack::packer<stream>&) {}

            template<typename T, typename... A>
            void write(msgpack::packer<stream>& pk, const T& arg, const A&... args)
            {
                pk.pack(arg);
                write(pk, args...);
            }

        public:
            writer(std::string& data) : data_(data) {}

            template<typename... A>
            void operator ()(const A&... args)
            {
                stream fw(data_);
                msgpack::packer<stream> pk(fw);
                write(pk, args...);
            }
        };

        class indexed_wrapper
        {
            friend class db;
        protected:
            db_layer<DB_DATA_LAYER>& db_;
            std::shared_ptr<batch_t> batch_;
            std::string data_;
        public:
            template<typename A>
            indexed_wrapper(db& db, const A& arg)
                : db_(db.db_)
                , batch_(db.batch())
            {
                stream s(data_);
                msgpack::packer<stream> pk(s);
                pk.pack(arg);
            }

            template<typename A>
            indexed_wrapper(const indexed_wrapper& iw, const A& arg)
                : db_(iw.db_)
                , batch_(iw.batch_)
                , data_(iw.data_)
            {
                stream s(data_);
                msgpack::packer<stream> pk(s);
                pk.pack(arg);
            }

            inline void batch(const std::shared_ptr<batch_t>& batch)
            {
                batch_ = batch;
            }

            template<typename A>
            inline indexed_wrapper operator [](const A& arg) const
            {
                return indexed_wrapper(*this, arg);
            }

            inline operator db()
            {
                return db(db_, data_.data(), data_.size());
            }

            inline operator bool()
            {
               std::string res;
               return db_.get(data_.data(), data_.size(), res);
            }

            template<typename A>
            bool operator >> (A& arg)
            {
                std::string res;
                if(db_.get(data_.data(), data_.size(), res))
                {
                    size_t off = 0;
                    msgpack::object_handle oh = msgpack::unpack(res.data(), res.size(), off);
                    msgpack::object obj = oh.get();
                    obj.convert(arg);
                    return off > 0;
                }
                return false;
            }

            bool operator = (std::nullptr_t)
            {
                db_layer<DB_DATA_LAYER> b;
                b.attach(&db_);
                if(batch_)
                    b.batch(batch_);
                return b.del(data_.data(), data_.size());
            }

            template<typename A>
            void operator = (const A& arg)
            {
                db_layer<DB_DATA_LAYER> b;
                b.attach(&db_);
                if(batch_)
                    b.batch(batch_);
                std::string val;
                stream s(val);
                msgpack::packer<stream> pk(s);
                pk.pack(arg);
                b.put(data_.data(), data_.size(), val.data(), val.size());
            }
        };

    public:
        db() {}
        db(const lldb::db& a) { db_.attach(&a); }
        db(const db& a) { db_.attach(&a.db_); }
        db(const std::string& path) { db_.open(path.c_str()); }
        db(const db& a, const char *pfx, size_t sz) { db_.attach(&a.db_, pfx, sz); }

        bool open(const std::string& path) { return db_.open(path.c_str()); }
        bool opened() { return db_.opened(); }

        std::shared_ptr<batch_t> batch() { return db_.batch(); }
        void batch(std::shared_ptr<batch_t> b) { db_.batch(b); }
        void commit() { db_.commit(); }

        operator const std::shared_ptr<::leveldb::DB>& () { return db_; }

        template<typename A>
        inline indexed_wrapper operator [](const A& arg)
        {
            return indexed_wrapper(*this, arg);
        }

        void operator = (const indexed_wrapper& w) { db_.attach(&w.db_, w.data_.data(), w.data_.size()); }

        struct out
        {
            size_t off;
            std::string data;
            bool is_valid;

            out()
                : off(0)
                , is_valid(false)
            {
            }

            template<typename A>
            out& operator >> (A& arg)
            {
                size_t o = off;
                msgpack::object_handle oh = msgpack::unpack(data.data(), data.size(), off);
                msgpack::object obj = oh.get();
                obj.convert(arg);
                is_valid = off > o;
                return *this;
            }

            inline operator bool()
            {
               return is_valid;
            }

            void clear()
            {
                data.clear();
                off = 0;
                is_valid = false;
            }
        };

        struct it_state;

        struct pair_t
        {
            out first, second;

            void clear()
            {
                first.clear();
                second.clear();
            }
        };

        STL_TYPEDEFS(pair_t);

        struct it_state {
            pair_t pair;
            bool is_valid, is_end;
            it_state()
                : is_valid(false)
                , is_end(false)
            {
            }
            void begin(const db* ref)
            {
                pair.clear();
                is_valid = ((db*)ref)->db_.first(pair.first.data, pair.second.data);
                if(!is_valid) is_end = true;
            }
            void find(const db* ref, const std::string key)
            {
                pair.clear();
                is_valid = ((db*)ref)->db_.seek(key.data(), key.size(), pair.first.data, pair.second.data);
                if(!is_valid) is_end = true;
            }
            void next(const db* ref)
            {
                pair.clear();
                is_valid = ((db*)ref)->db_.next(pair.first.data, pair.second.data);
                if(!is_valid) is_end = true;
            }
            void next(const db* ref, int n)
            {
                pair.clear();
                is_valid = ((db*)ref)->db_.skip(n, nullptr, 0, pair.first.data, pair.second.data, false);
                if(!is_valid) is_end = true;
            }
            inline void end(const db*)
            {
                pair.clear();
                is_valid = false;
                is_end = true;
            }
            inline pair_t& get(const db*)
            {
                return pair;
            }
            inline bool cmp(const it_state& s) const
            {
                return is_valid != s.is_valid && is_end != s.is_end;
            }
            void prev(const db* ref)
            {
                pair.clear();
                if(is_end)
                    is_valid = ((db*)ref)->db_.last(pair.first.data, pair.second.data);
                else
                    is_valid = ((db*)ref)->db_.prev(pair.first.data, pair.second.data);
                if(is_valid) is_end = false;
            }
            void prev(const db* ref, int n)
            {
                pair.clear();
                if(is_end)
                    is_valid = ((db*)ref)->db_.skip(-n, nullptr, 0, pair.first.data, pair.second.data, is_end);
                if(is_valid) is_end = false;
            }
            inline const pair_t& get(const db*) const
            {
                return pair;
            }
        };

        // Declare typedef ... iterator;, begin() and end() functions:
        SETUP_ITERATORS(db, pair_t&, it_state)
        SETUP_REVERSE_ITERATORS(db, pair_t&, it_state)

        template<typename... A>
        iterator find(const A&... args)
        {
            std::string key;
            writer w(key); w(args...);
            return iterator::find(this, key);
        }

        template<typename... A>
        const_iterator cfind(const A&... args)
        {
            std::string key;
            writer w(key); w(args...);
            return const_iterator::find(this, key);
        }
    };

}}

#endif
