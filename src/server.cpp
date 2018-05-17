#include "CmdLine.h"
#include "Config.h"
#include "Logger.h"
#include "Router.h"
#include "SignalHandler.h"

#include <unistd.h>

/*
 * "main" - should use other modules for everything, that is:
 *    - read cmd line options
 *    - read config file
 *    - bind the router tho the receiving port and give it control
 *    - when router finishes, log and shutdown
 */

int main() {
    // Init basic classes
    CmdLine cmdline;
    Config* config = Config::getInstance();
    Logger logger("Server");
    SignalHandler signalHandler;

    logger.info("Process id: " + std::to_string(getpid()));
    if(config->get_str_setting("start_full_server") == "true") {
        // starting the full server
        std::string port = config->get_str_setting("port");
        int queue_size   = config->get_int_setting("queue_size");
        logger.info("Starting HackTTP at port: " + port);

        try {
            Router router(queue_size, port);
            router.watch();
        }
        catch (BaseException &e) {
            logger.warn( std::string(e.what()) + ", terminating server..." );
        }
    } else {
        logger.info("Running in test mode");
    }
    // cleanup

    logger.info("HackTTP shutting down");

    return 0;
}
