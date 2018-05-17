#include "Router.h"

#include <unistd.h>
#include <iostream>

int main() {
    std::cout << "Running server.." << std::endl;
    try {
        Router router(20, 8000);
        router.watch();
    } catch (BaseException &e) {
        std::cerr << "Exception: " << std::string(e.what()) << std::endl;
    }
    return 0;
}
