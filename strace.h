#ifndef __STRACE_H__
#define __STRACE_H__

int tracee(char** cmd);
int tracer(pid_t child_pid);

#endif