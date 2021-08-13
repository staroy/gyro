#include "lsrv.h"
#include <exception>
#include <boost/asio/signal_set.hpp>

int main(int argc, char *argv[])
{
  try
  {
    // disable default handler
    zsys_handler_set(nullptr);

    boost::asio::io_service ios;
    lzyre::lsrv S(ios);
    
    boost::asio::signal_set exit(ios, SIGINT, SIGTERM);
    exit.async_wait([&](boost::system::error_code const& /*e*/, int /*s*/) {
      S.stop();
      ios.stop();
    });

    S.start(argc, argv);
    S.join();
  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << "\n";
  }

  return 0;
}
