namespace cryptonote {

  class console_handlers_binder : public command_handler
  {
    typedef command_handler::callback console_command_handler;
    typedef command_handler::lookup command_handlers_map;
    std::unique_ptr<boost::thread> m_console_thread;
    async_console_handler m_console_handler;
  public:
    ~console_handlers_binder() {
      try
      {
        stop_handling();
        if (m_console_thread.get() != nullptr)
        {
          m_console_thread->join();
        }
      }
      catch (const std::exception &e) { /* ignore */ }
    }

    bool start_handling(std::function<std::string(void)> prompt, const std::string& usage_string = "", std::function<void(void)> exit_handler = NULL)
    {
      m_console_thread.reset(new boost::thread(boost::bind(&console_handlers_binder::run_handling, this, prompt, usage_string, exit_handler)));
      return true;
    }
    bool start_handling(const std::string &prompt, const std::string& usage_string = "", std::function<void(void)> exit_handler = NULL)
    {
      return start_handling([prompt](){ return prompt; }, usage_string, exit_handler);
    }

    void stop_handling()
    {
      m_console_handler.stop();
    }

    bool run_handling(std::function<std::string(void)> prompt, const std::string& usage_string, std::function<void(void)> exit_handler = NULL)
    {
      return m_console_handler.run(boost::bind(&console_handlers_binder::process_command_str, this, boost::placeholders::_1), prompt, usage_string, exit_handler);
    }

    void print_prompt()
    {
      m_console_handler.print_prompt();
    }

    void cancel_input()
    {
      m_console_handler.cancel();
    }
  };
}