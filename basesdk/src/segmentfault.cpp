#include "log.h"
#include "segmentfault.h"

namespace baseservice
{

long int gettid() {
#ifndef  WIN32
    return syscall(224);
#else
	return 0;
#endif
}
char znsegfault_catch::handlesvaule[100][20] = {
    "NOSIGNAL",
    "SIGHUP",
    "SIGINT",
    "SIGQUIT",
    "SIGILL",
    "SIGTRAP",
    "SIGABRT",
    "SIGBUS",
    "SIGFPE",
    "SIGKILL",
    "SIGUSR1",
    "SIGSEGV",
    "SIGUSR2",
    "SIGPIPE",
    "SIGALRM",
    "SIGTERM",
    "SIGSTKFLT",
    "SIGCHLD",
    "SIGCONT",
    "SIGSTOP",
    "SIGTSTP",
    "SIGTTIN",
    "SIGTTOU",
    "SIGURG",
    "SIGXCPU",
    "SIGXFSZ",
    "SIGVTALRM",
    "SIGPROF",
    "SIGWINCH",
    "SIGIO",
    "SIGPWR",
    "SIGSYS",
    "SIGRTMIN",
    "SIGRTMIN+1",
    "SIGRTMIN+2",
    "SIGRTMIN+3",
    "SIGRTMIN+4",
    "SIGRTMIN+5",
    "SIGRTMIN+6",
    "SIGRTMIN+7",
    "SIGRTMIN+8",
    "SIGRTMIN+9",
    "SIGRTMIN+10",
    "SIGRTMIN+11",
    "SIGRTMIN+12",
    "SIGRTMIN+13",
    "SIGRTMIN+14",
    "SIGRTMIN+15",
    "SIGRTMAX-14",
    "SIGRTMAX-13",
    "SIGRTMAX-12",
    "SIGRTMAX-11",
    "SIGRTMAX-10",
    "SIGRTMAX-9",
    "SIGRTMAX-8",
    "SIGRTMAX-7",
    "SIGRTMAX-6",
    "SIGRTMAX-5",
    "SIGRTMAX-4",
    "SIGRTMAX-3",
    "SIGRTMAX-2",
    "SIGRTMAX-1",
    "SIGRTMAX",
};

TISL znsegfault_catch::m_thread_info;
void znsegfault_catch::handle() {
#ifdef ZLINUX
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0) {
        SYS_LOG(ZLOGINFO, "block sigpipe ZLOGERROR\n");
    }
#endif
}
void znsegfault_catch::catch_sign(void) {
#ifdef ZLINUX
    handle();
    signal(SIGSEGV, print_seg_frame);
    signal(SIGINT, print_seg_frame);
    signal(SIGABRT, print_seg_frame);
    signal(SIGPIPE, no_print_seg_frame);
#endif
}
void znsegfault_catch::no_print_seg_frame(int signal) {
    //SYS_LOG(ZLOGERROR,"receive %s\n",handlesvaule[signal]);
    return;
}
void znsegfault_catch::setup_thread_info(unsigned int tid, const char *tname) {
    TIS info;
    strncpy(info.name, tname, 255);
    info.tid = tid;
    m_thread_info.push_back(info);
}
TIS znsegfault_catch::getThreadInfo() {
    pid_t tid = gettid();
    auto iter = m_thread_info.begin();
    while (iter != m_thread_info.end()) {
        if (tid == iter->tid)
            return *iter;
        iter++;
    }
    TIS info;
    info.tid = 0;
    strcpy(info.name, "unkonw");
    return info;
}
void znsegfault_catch::print_seg_frame(int signal) {
#ifdef ZLINUX
    TIS tis = getThreadInfo();
    void *buffer[30] = {0};
    size_t size;
    char **strings = NULL;
    size_t i = 0;

    size = backtrace(buffer, 30);
    //fprintf(stdout, "Obtained %zd stack frames\n", size);
    SYS_LOG(ZLOGERROR, "thread %d [%s] Receive %s:\t%zd stack frames\n", tis.tid, tis.name, handlesvaule[signal], size);
    strings = backtrace_symbols(buffer, size);
    if (strings == NULL) {
        SYS_LOG(ZLOGERROR, "backtrace_symbols.");
        //exit(EXIT_FAILURE);
    }

    for (i = 0; i < size; i++) {
        //fprintf(stdout, "%s\n", strings[i]);
        SYS_LOG(ZLOGERROR, "%s\n", strings[i]);
    }
    free(strings);
    strings = NULL;
    exit(0);
#else
    unsigned int tid = 0;
#endif
}

}; // namespace baseservice
