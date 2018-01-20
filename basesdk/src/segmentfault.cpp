#include "log.h"
#include "segmentfault.h"
namespace baseservice
{
    
    char znsegfault_catch::handlesvaule[100][20]={
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
    void znsegfault_catch::handle()
     {
         sigset_t signal_mask;
         sigemptyset (&signal_mask);
         sigaddset (&signal_mask, SIGPIPE);
         int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
         if (rc != 0)
        {
              SYS_LOG(INFO,"block sigpipe error\n");
         }
     }
    void znsegfault_catch::catch_sign(void)
    {
        handle();
         signal(SIGSEGV, print_seg_frame);     
        signal(SIGINT, print_seg_frame);
        signal(SIGABRT, print_seg_frame);
        signal(SIGPIPE, no_print_seg_frame);
    }
    void znsegfault_catch::no_print_seg_frame(int signal)
    {
         SYS_LOG(ERROR,"receive %s\n",handlesvaule[signal]);
        return ;
    }
    void znsegfault_catch::print_seg_frame(int signal)
    {
          void *buffer[30] = {0};
          size_t size;
          char **strings = NULL;
          size_t i = 0;

          size = backtrace(buffer, 30);
          //fprintf(stdout, "Obtained %zd stack frames\n", size);
        SYS_LOG(ERROR, "Receive %s:\t%zd stack frames\n", handlesvaule[signal],size);
          strings = backtrace_symbols(buffer, size);
          if (strings == NULL)
          {
               perror("backtrace_symbols.");
               //exit(EXIT_FAILURE);
          }

          for (i = 0; i < size; i++)
          {
               //fprintf(stdout, "%s\n", strings[i]);
                SYS_LOG(ERROR, "%s\n", strings[i]);
          }
          free(strings);
          strings = NULL;
        exit(0);
     }

};

