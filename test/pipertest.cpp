//#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>
#include "DualPipe.h"

int main(int argc, char* argv[]) {
  std::list<std::string> args;
  if (argc < 3) {
    std::cerr << "not enough args" << std::endl;
    return 1;
  }
  for (int i = 2; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  std::thread worker([&](){ 
  for (auto arg : args) {
    DualPipe pipe;
    std::list<std::string> inv = { argv[1], "24", "28" }; 
    int pid = pipe.open(inv);
    std::cout << "opened pid " << pid << std::endl;
    for (auto val : args) {
      pipe.write(val);
      pipe.write(" ");
      ::sleep(10);
    }
    pipe.close();
  }
  });
  worker.join();
  return 0;
}
