#include "Router.h"

#include <unistd.h>
#include <iostream>

int main(int argc, char **argv) {
  int port = 8000;
  if (argc > 1) port = atoi(argv[1]);

  std::cout << "Listening " << port << std::endl;
  try {
    // qsize, port
    Router router(20, port);
    router.watch();
  } catch (BaseException &e) {
    std::cerr << "Exception: " << std::string(e.what()) << std::endl;
  }
  return 0;
}
