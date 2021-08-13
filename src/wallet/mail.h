#ifndef __gyro_message_mail_h__
#define __gyro_message_mail_h__

#include <string>
#include <atomic>

namespace gyro { namespace sms { namespace mail {

    class server
    {
      short  pop3_port_, smtp_port_;
      std::string data_path_;
      std::atomic<bool> run_;
      std::thread thread_;
      void run();
    public:
      server(short pop3_port = 0, short smtp_port = 0, const std::string& data_path = "message.data");
      void set_ports(short pop3_port = 0, short smtp_port = 0);
      void set_path(const std::string& path);
      bool start();
      bool stop();
      bool is_run();
    };

}}}

#endif
