#ifndef JUCI_PROJECT_H_
#define JUCI_PROJECT_H_

#include <gtkmm.h>
#include "notebook.h"
#include "cmake.h"
#include <boost/filesystem.hpp>
#include "directories.h"
#include <atomic>
#include <mutex>
#include "tooltips.h"

class Project {
private:
  std::pair<boost::filesystem::path, std::pair<int, int> > debug_stop;
  std::mutex debug_stop_mutex;
  boost::filesystem::path debug_last_stop_file_path;
  Glib::Dispatcher debug_update_stop;
  std::string debug_status;
  std::mutex debug_status_mutex;
  Glib::Dispatcher debug_update_status;

  Project();
  Notebook &notebook; //convenience reference
public:
  static Project &get() {
    static Project singleton;
    return singleton;
  }

  Gtk::Label debug_status_label;
  std::unordered_map<std::string, std::string> run_arguments;
  std::unordered_map<std::string, std::string> debug_run_arguments;
  std::atomic<bool> compiling;
  std::atomic<bool> debugging;
  
  class Language {
  protected:
    Notebook &notebook; //convenience reference
  public:
    Language() : notebook(Notebook::get()) {}
    virtual ~Language() {}
    
    virtual std::pair<std::string, std::string> get_run_arguments() {return {"", ""};}
    virtual void compile() {}
    virtual void compile_and_run() {}
    
    virtual std::pair<std::string, std::string> debug_get_run_arguments() {return {"", ""};}
    Tooltips debug_variable_tooltips;
    virtual void debug_start() {}
    virtual void debug_continue() {}
    virtual void debug_stop() {}
    virtual void debug_kill() {}
    virtual void debug_step_over() {}
    virtual void debug_step_into() {}
    virtual void debug_step_out() {}
    virtual void debug_backtrace() {}
    virtual void debug_show_variables() {}
    virtual void debug_run_command(const std::string &command) {}
    virtual void toggle_breakpoint() {}
    virtual void debug_goto_stop() {}
    virtual void debug_delete() {}
  };
  
  class Clang : public Language {
  public:
    Clang() : Language() {}
    
    std::unique_ptr<CMake> get_cmake();
    
    std::pair<std::string, std::string> get_run_arguments() override;
    void compile() override;
    void compile_and_run() override;
    
    std::mutex debug_start_mutex;
  #ifdef JUCI_ENABLE_DEBUG
    std::pair<std::string, std::string> debug_get_run_arguments() override;
    void debug_start() override;
    void debug_continue() override;
    void debug_stop() override;
    void debug_kill() override;
    void debug_step_over() override;
    void debug_step_into() override;
    void debug_step_out() override;
    void debug_backtrace() override;
    void debug_show_variables() override;
    void debug_run_command(const std::string &command) override;
    void toggle_breakpoint() override;
    void debug_goto_stop() override;
    void debug_delete() override;
  #endif
  };
  
  class Markdown : public Language {
  public:
    Markdown() : Language() {}
    ~Markdown();
    
    boost::filesystem::path last_temp_path;
    void compile_and_run() override;
  };
  
  class Python : public Language {
  public:
    Python() : Language() {}
    
    void compile_and_run() override;
  };
  
  class JavaScript : public Language {
  public:
    JavaScript() : Language() {}
    
    void compile_and_run() override;
  };
  
  class HTML : public Language {
  public:
    HTML() : Language() {}
    
    void compile_and_run() override;
  };
  
  std::unique_ptr<Language> get_language();
};

#endif  // JUCI_PROJECT_H_