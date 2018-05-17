#include "Router.h"
#include "SignalHandler.h"

#include <unistd.h>
#include <iostream>

int main() {
  signal_init();

  std::cout << "Running server.." << std::endl;
  try {
    Router router(20, 8000); // qsize, port
    router.watch();
  } catch (BaseException &e) {
    std::cerr << "Exception: " << std::string(e.what()) << std::endl;
  }
  return 0;
}
