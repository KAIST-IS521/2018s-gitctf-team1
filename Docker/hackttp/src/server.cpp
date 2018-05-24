#include "Router.h"

#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[]) {
  int port = 8000;
  if (argc < 2) return 0;
  else if (argc > 2) port = atoi(argv[2]);

  std::cout << "Listening " << port << std::endl;
  try {
    // qsize, port
    Router router(20, port, argv[1]);
    router.watch();
  } catch (BaseException &e) {
    std::cerr << "Exception: " << std::string(e.what()) << std::endl;
  }
  return 0;
}
