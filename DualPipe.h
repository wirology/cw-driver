#pragma once

#include <string>
#include <list>

class piper_exception : public std::exception {
public:
  piper_exception(const std::string message) : _message(message) {}
  const char* what() {
    return _message.c_str();
  }
private:
  std::string _message;
};
 
class DualPipe {
public:
  DualPipe();
  // non copyable
  DualPipe(const DualPipe&) = delete;
  DualPipe(DualPipe&&) = delete;
  DualPipe& operator=(const DualPipe&) = delete;
  DualPipe& operator=(DualPipe&&) = delete;

  ~DualPipe();
  int open(const std::list<std::string>& argv);
  std::pair<std::string,int> close();
  void kill();
  void write(const std::string& message);
  std::string read() const;

private:
  int _childpid;
  int _fd_p2c[2]; 
  int _fd_c2p[2];
};

