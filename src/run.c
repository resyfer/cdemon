#include <include/run.h>
#include <stdio.h>
#include <include/global.h>
#include <sys/signal.h>
#include <include/util.h>
#include <sys/wait.h>

int proc = 0;

void
run()
{
	if(proc != 0) {
		kill(proc, SIGINT);
	}
	int pid = fork();

	if(pid == -1) {
		error("Can't fork.")
	} else if (pid == 0) {
		execvp(command, args);
	} else {
		proc = pid; // child's PID
	}
}