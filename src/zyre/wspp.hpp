#ifndef __zyre_ws_h__
#define __zyre_ws_h__

#include <boost/asio/io_service.hpp>

#include "websocketpp/random/random_device.hpp"
#include "websocketpp/config/asio.hpp"
#include "websocketpp/config/asio_client.hpp"
#include <websocketpp/connection_base.hpp>
#include "websocketpp/server.hpp"
#include "websocketpp/client.hpp"
#include <websocketpp/logger/levels.hpp>
#include <websocketpp/common/cpp11.hpp>
#include <websocketpp/common/stdint.hpp>
#include <websocketpp/common/time.hpp>

#include "misc_log_ex.h"

namespace websocketpp {
namespace log {

    template <typename concurrency, typename names>
    class easylogging {
    public:
        easylogging<concurrency,names>(channel_type_hint::value h =
            channel_type_hint::access)
          : m_static_channels(0xffffffff)
          , m_dynamic_channels(0)
        {}

        easylogging<concurrency,names>(std::ostream * out)
          : m_static_channels(0xffffffff)
          , m_dynamic_channels(0)
        {}

        easylogging<concurrency,names>(level c, channel_type_hint::value h =
            channel_type_hint::access)
          : m_static_channels(c)
          , m_dynamic_channels(0)
        {}

        easylogging<concurrency,names>(level c, std::ostream * out)
          : m_static_channels(c)
          , m_dynamic_channels(0)
        {}

        /// Destructor
        ~easylogging<concurrency,names>()
        {}

        /// Copy constructor
        easylogging<concurrency,names>(easylogging<concurrency,names> const & other)
         : m_static_channels(other.m_static_channels)
         , m_dynamic_channels(other.m_dynamic_channels)
        {}

#ifdef _WEBSOCKETPP_DEFAULT_DELETE_FUNCTIONS_
        // no copy assignment operator because of const member variables
        easylogging<concurrency,names> & operator=(easylogging<concurrency,names> const &) = delete;
#endif // _WEBSOCKETPP_DEFAULT_DELETE_FUNCTIONS_

#ifdef _WEBSOCKETPP_MOVE_SEMANTICS_
        /// Move constructor
        easylogging<concurrency,names>(easylogging<concurrency,names> && other)
            : m_static_channels(other.m_static_channels)
            , m_dynamic_channels(other.m_dynamic_channels)
        {}

#ifdef _WEBSOCKETPP_DEFAULT_DELETE_FUNCTIONS_
        // no move assignment operator because of const member variables
        easylogging<concurrency,names> & operator=(easylogging<concurrency,names> &&) = delete;
#endif // _WEBSOCKETPP_DEFAULT_DELETE_FUNCTIONS_

#endif // _WEBSOCKETPP_MOVE_SEMANTICS_

        void set_ostream(std::ostream * out = &std::cout) {
        }

        void set_channels(level channels) {
            if (channels == names::none) {
                clear_channels(names::all);
                return;
            }

            scoped_lock_type lock(m_lock);
            m_dynamic_channels |= (channels & m_static_channels);
        }

        void clear_channels(level channels) {
            scoped_lock_type lock(m_lock);
            m_dynamic_channels &= ~channels;
        }

        /// Write a string message to the given channel
        /**
         * @param channel The channel to write to
         * @param msg The message to write
         */
        void write(level channel, std::string const & msg) {
            scoped_lock_type lock(m_lock);
            if (!this->dynamic_test(channel)) { return; }
            MLOG_CYAN(el::Level::Info, msg);
            /*if(channel & elevel::warn)
              MLOG_CYAN(el::Level::Warning, msg);
            else if(channel & elevel::info)
              MLOG_CYAN(el::Level::Info, msg);
            else if(channel & elevel::devel)
              MLOG_CYAN(el::Level::Debug, msg);
            else if(channel & elevel::fatal)
              MLOG_CYAN(el::Level::Fatal, msg);
            else if(channel & elevel::rerror)
              MLOG_CYAN(el::Level::Error, msg);
            else if(channel & elevel::library)
              MLOG_CYAN(el::Level::Trace, msg);
            else
              MLOG_CYAN(el::Level::Unknown, msg);*/
        }

        /// Write a cstring message to the given channel
        /**
         * @param channel The channel to write to
         * @param msg The message to write
         */
        void write(level channel, char const * msg) {
            scoped_lock_type lock(m_lock);
            if (!this->dynamic_test(channel)) { return; }
            MLOG_CYAN(el::Level::Info, msg);
            /*if(channel & elevel::warn)
              MLOG_CYAN(el::Level::Warning, msg);
            else if(channel & elevel::info)
              MLOG_CYAN(el::Level::Info, msg);
            else if(channel & elevel::devel)
              MLOG_CYAN(el::Level::Debug, msg);
            else if(channel & elevel::fatal)
              MLOG_CYAN(el::Level::Fatal, msg);
            else if(channel & elevel::rerror)
              MLOG_CYAN(el::Level::Error, msg);
            else if(channel & elevel::library)
              MLOG_CYAN(el::Level::Trace, msg);
            else
              MLOG_CYAN(el::Level::Unknown, msg);*/
        }

        _WEBSOCKETPP_CONSTEXPR_TOKEN_ bool static_test(level channel) const {
            return ((channel & m_static_channels) != 0);
        }

        bool dynamic_test(level channel) {
            return ((channel & m_dynamic_channels) != 0);
        }

    protected:
        typedef typename concurrency::scoped_lock_type scoped_lock_type;
        typedef typename concurrency::mutex_type mutex_type;
        mutex_type m_lock;

    private:
        // The timestamp does not include the time zone, because on Windows with the
        // default registry settings, the time zone would be written out in full,
        // which would be obnoxiously verbose.
        //
        // TODO: find a workaround for this or make this format user settable
        static std::ostream & timestamp(std::ostream & os) {
            std::time_t t = std::time(NULL);
            std::tm lt = lib::localtime(t);
            #ifdef _WEBSOCKETPP_PUTTIME_
                return os << std::put_time(&lt,"%Y-%m-%d %H:%M:%S");
            #else // Falls back to strftime, which requires a temporary copy of the string.
                char buffer[20];
                size_t result = std::strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",&lt);
                return os << (result == 0 ? "Unknown" : buffer);
            #endif
        }

        level const m_static_channels;
        level m_dynamic_channels;
    };
} // log

namespace config {
    struct server_tls : public core {
        typedef asio_tls type;
        typedef core base;

        typedef base::concurrency_type concurrency_type;

        typedef base::request_type request_type;
        typedef base::response_type response_type;

        typedef base::message_type message_type;
        typedef base::con_msg_manager_type con_msg_manager_type;
        typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::alevel> alog_type;
        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::elevel> elog_type;

        typedef base::rng_type rng_type;

        struct transport_config : public base::transport_config {
            typedef type::concurrency_type concurrency_type;
            typedef server_tls::alog_type alog_type;
            typedef server_tls::elog_type elog_type;
            typedef type::request_type request_type;
            typedef type::response_type response_type;
            typedef websocketpp::transport::asio::tls_socket::endpoint socket_type;
        };
        typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
        typedef websocketpp::connection_base connection_base;
    };

    struct server : public core {
        typedef asio type;
        typedef core base;

        typedef base::concurrency_type concurrency_type;

        typedef base::request_type request_type;
        typedef base::response_type response_type;

        typedef base::message_type message_type;
        typedef base::con_msg_manager_type con_msg_manager_type;
        typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::alevel> alog_type;
        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::elevel> elog_type;

        typedef base::rng_type rng_type;

        struct transport_config : public base::transport_config {
            typedef type::concurrency_type concurrency_type;
            typedef server::alog_type alog_type;
            typedef server::elog_type elog_type;
            typedef type::request_type request_type;
            typedef type::response_type response_type;
            typedef websocketpp::transport::asio::basic_socket::endpoint socket_type;
        };
        typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
        typedef websocketpp::connection_base connection_base;
    };

    struct client_tls : public core_client {
        typedef asio_tls_client type;
        typedef core_client base;

        typedef base::concurrency_type concurrency_type;

        typedef base::request_type request_type;
        typedef base::response_type response_type;

        typedef base::message_type message_type;
        typedef base::con_msg_manager_type con_msg_manager_type;
        typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::alevel> alog_type;
        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::elevel> elog_type;

        typedef base::rng_type rng_type;

        struct transport_config : public base::transport_config {
            typedef type::concurrency_type concurrency_type;
            typedef client_tls::alog_type alog_type;
            typedef client_tls::elog_type elog_type;
            typedef type::request_type request_type;
            typedef type::response_type response_type;
            typedef websocketpp::transport::asio::tls_socket::endpoint socket_type;
        };
        typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
        typedef websocketpp::connection_base connection_base;
    };
    struct client : public core_client {
        typedef asio_client type;
        typedef core_client base;

        typedef base::concurrency_type concurrency_type;

        typedef base::request_type request_type;
        typedef base::response_type response_type;

        typedef base::message_type message_type;
        typedef base::con_msg_manager_type con_msg_manager_type;
        typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::alevel> alog_type;
        typedef websocketpp::log::easylogging<concurrency_type, websocketpp::log::elevel> elog_type;

        typedef base::rng_type rng_type;

        struct transport_config : public base::transport_config {
            typedef type::concurrency_type concurrency_type;
            typedef client::alog_type alog_type;
            typedef client::elog_type elog_type;
            typedef type::request_type request_type;
            typedef type::response_type response_type;
            typedef websocketpp::transport::asio::basic_socket::endpoint socket_type;
        };
        typedef websocketpp::transport::asio::endpoint<transport_config> transport_type;
        typedef websocketpp::connection_base connection_base;
    };
}}

#endif
