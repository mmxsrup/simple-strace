#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strace.h"
#include "syscall_table.h"


int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: ./strace PROG [ARGS] or ./strace -p PID\n");
		exit(1);
	}

	if (strcmp(argv[1], "-p") == 0) {
		pid_t tracee_pid = atoi(argv[2]);
		if (ptrace(PTRACE_ATTACH, tracee_pid, NULL, NULL) < 0) {
			perror("Failed ptrace(PTRACE_ATTACH, pid)");
		}
		tracer(tracee_pid);
	} else {
		pid_t pid = fork();
		if (pid == -1) {
			perror("Failed frok");
			exit(1);
		} else if (pid == 0) { // child
			tracee(argv + 1);
		} else {
			tracer(pid);
		}
	}

	return 0;
}

int tracee(char** cmd) {
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
		perror("Failed ptrace(PTRACE_TRACEME");
		exit(1);
	}
	return execvp(cmd[0], cmd);
}

int tracer(pid_t tracee_pid) {
	int status;
	waitpid(tracee_pid, &status, 0); // wait for execvp

	while (1) {
		if (ptrace(PTRACE_SYSCALL, tracee_pid, NULL, NULL) < 0) {
			perror("Failed ptrace(PTRACE_SYSCALL");
			exit(1);
		}

		int status;
		waitpid(tracee_pid, &status, 0); // Wait for syscall enter or exit

		if (WIFEXITED(status)) // Child terminated normally
			break;

		struct user_regs_struct regs;
		if (ptrace(PTRACE_GETREGS, tracee_pid, NULL, &regs) < 0) {
			perror("Faild ptrace(PTRACE_GETREGS)");
			exit(1);
		}

		// Check enter-stop or exit-stop
		if (regs.rax != -ENOSYS) { // Is enter-stop
			continue;
		}

		int syscall_num = regs.orig_rax;
		printf("%s = %d\n", syscall_table[syscall_num], syscall_num);
	}
	return 0;
}