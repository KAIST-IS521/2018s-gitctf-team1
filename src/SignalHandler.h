#pragma once

#include "Common.h"
#include <signal.h>

extern bool isSigintReceived;
extern bool isSigusr1Received;

void signal_init();
void sigint_handler (int sig, siginfo_t *siginfo, void *context);
void sigusr1_handler(int sig, siginfo_t *siginfo, void *context);
void sigchld_handler(int sig, siginfo_t *siginfo, void *context);
