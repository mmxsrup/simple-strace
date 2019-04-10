#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "strace.h"


int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: ./strace [prog] [args]\n");
		exit(1);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("Failed frok");
		exit(1);
	} else if (pid == 0) { // child
		tracee(argv + 1);
	} else {
		tracer(pid);
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

int tracer(pid_t child_pid) {
	int status;
	waitpid(child_pid, &status, 0); // wait for execvp

	while (1) {
		if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) < 0) {
			perror("Failed ptrace(PTRACE_SYSCALL");
			exit(1);
		}

		int status;
		waitpid(child_pid, &status, 0); // Wait for syscall enter or exit

		if (WIFEXITED(status)) // Child terminated normally
			break;

		struct user_regs_struct regs;
		if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) < 0) {
			perror("Faild ptrace(PTRACE_GETREGS)");
			exit(1);
		}

		// Check enter-stop or exit-stop
		if (regs.rax != -ENOSYS) { // Is enter-stop
			continue;
		}

		printf("Syscall num: %llx \n", regs.orig_rax);
	}
	return 0;
}