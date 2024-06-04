#include "DualPipe.h"
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <cstring>
#include <vector>
#include <iostream>

DualPipe::DualPipe() : _childpid(-1) {
    std::memset(_fd_p2c, 0, sizeof(_fd_p2c));
    std::memset(_fd_c2p, 0, sizeof(_fd_c2p));
}

DualPipe::~DualPipe() {
  close();
}

int DualPipe::open(const std::list<std::string>& argv) {
  if (argv.empty()) {
    throw piper_exception("open piper missing arguments");
  }
  // fd[0] is read and fd[1] write descriptor
  if (::pipe(_fd_p2c) != 0 || ::pipe(_fd_c2p) != 0) {
    throw piper_exception("failed to open pipes");
  }
  _childpid = fork();

  if (_childpid < 0)
  {
    throw piper_exception("failed to fork");
  }
  else if (_childpid == 0) {
    if (::close(_fd_p2c[1]) != 0) {
      std::cerr << "child failed to close p2c write FD" << std::endl;
    }
    if (::close(_fd_c2p[0]) != 0) {
      std::cerr << "child failed to close c2p read FD" << std::endl;
    }
    if (::dup2(_fd_p2c[0], STDIN_FILENO) != STDIN_FILENO) {
      std::cerr << "child failed to redirect p2c read FD to STDIN" << std::endl;
    } 
    if (::dup2(_fd_c2p[1], STDOUT_FILENO) != STDOUT_FILENO) {
      std::cerr << "child failed to redirect c2p write FD to STDOUT" << std::endl;
    }
    if (::close(_fd_p2c[0]) != 0 || ::close(_fd_c2p[1]) != 0) {
      std::cerr << "child failed to close redirected p2c read and c2p write FDs" << std::endl; 
    }

    std::vector<char*> exec_args(argv.size()+1);
    std::vector<char*>::iterator iter = exec_args.begin();
    for (const auto& arg : argv) {
      *iter++ = const_cast<char*>(arg.c_str());
    }
    *iter = reinterpret_cast<char*>(0); 
    // allow for child process tree kill
    ::setpgid(0,0);
    ::execv(argv.front().c_str(), exec_args.data());
    std::cerr << "Failed to execute " << argv.front() << std::endl;
    ::exit(1);
  } else {
    if (::close(_fd_p2c[0]) != 0) {
      throw piper_exception("parent failed to close p2c read FD");
    } 
    if (::close(_fd_c2p[1]) != 0) {
      throw piper_exception("parent failed to close c2p write FD");
    }
  }
  return _childpid;
}

std::pair<std::string,int> DualPipe::close() {
  if (_fd_p2c[1] == 0) {
    return std::make_pair<std::string,int>("closed already",0);
  }
  // Closing the parent to child write pipe
  // terminates child
  if (::close(_fd_p2c[1]) != 0) {
    std::cerr << "parent error closing p2c write FD" << std::endl;
  }
  _fd_p2c[1] = 0;
  std::string receive_output = read();
  if (::close(_fd_c2p[0]) != 0) {
    std::cerr << "parent error closing c2p read FD" << std::endl;
  }
  // waitpid needed to avoid defunct child process hanging on
  int status = 0;
  ::waitpid(_childpid, &status, 1);
  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
  } else {
    status = -1;
  }
  return std::make_pair<std::string,int>(std::move(receive_output), std::move(status));
} 

void DualPipe::kill() {
  if (_childpid > -1) {
    ::kill(-_childpid, SIGTERM);
  }
}

void DualPipe::write(const std::string& message) {
  int nbytes = message.length();
  if (::write(_fd_p2c[1], message.c_str(), nbytes) != nbytes)
  {
    throw piper_exception("Parent fails to write to child");
  }
}

std::string DualPipe::read() const {
  std::string receive_output;
  while (1)
  {
    char readbuffer[80];
    int bytes_read = ::read(_fd_c2p[0], readbuffer, sizeof(readbuffer)-1);

    if (bytes_read <= 0)
      break;

    readbuffer[bytes_read] = '\0';
    receive_output += readbuffer;
  }
  return receive_output;
}

