#include "SignalHandler.h"

bool isSigintReceived;
bool isSigusr1Received;

void signal_init() {
  {
    isSigintReceived = false;

    struct sigaction sig;
    memset (&sig, 0, sizeof(sig));
    sig.sa_sigaction = &sigintHandler;
    sig.sa_flags = SA_SIGINFO;

    if (sigaction(SIGINT, &sig, NULL) < 0) {
      perror("sigaction_SIGINT");
      exit(1);
    }
  }

  {
    isSigusr1Received = false;

    struct sigaction sig;
    memset (&sig, '\0', sizeof(sig));
    sig.sa_sigaction = &sigusr1Handler;
    sig.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGUSR1, &sig, NULL) < 0) {
      perror("sigaction_SIGUSR1");
      exit(1);
    }
  }

  {
    struct sigaction sig;
    memset (&sig, '\0', sizeof(sig));
    sig.sa_sigaction = &sigchldHandler;
    sig.sa_flags = SA_SIGINFO;

    if (sigaction(SIGCHLD, &sig, NULL) < 0) {
      perror("sigaction_SIGCHLD");
      exit(1);
    }
  }
}


void sigint_handler(int sig, siginfo_t *siginfo, void *context) {
  isSigintReceived = true;
}

void sigusr1_handler(int sig, siginfo_t *siginfo, void *context) {
  isSigusr1Received = true;
}

void sigchld_handler(int sig, siginfo_t *siginfo, void *context) {
  fprintf(stderr, "Child Exit\n");
  while (waitpid(-1, NULL, WNOHANG) > 0) { }
}
