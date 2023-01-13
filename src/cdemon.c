#include <include/watch.h>
#include <include/util.h>
#include <include/global.h>
#include <sys/signal.h>
#include <string.h>
#include <libvector/vector.h>
#include <sys/inotify.h>

#include <unistd.h>
#include <libtpool/tpool.h>

#include <stdio.h>

char *command;
char **args;
int parent;

extern int notify_fd;
extern vector_t *notify_wds;
extern int proc;

void shutdown(int sig) {

	if(getpid() != parent) {
		return;
	}

	success("\n[cdemon] Shutting down cdemon...");

	kill(proc, SIGKILL);

	int n = vec_size(notify_wds);
	for(int i = 0; i<n; i++) {
		int *wd = vec_get(notify_wds, i);

		if(!wd) {
			continue;
		}

		inotify_rm_watch(notify_fd, *wd);
	}
	close(notify_fd);
	success("[cdemon] Shutdown complete.");
	exit(0);

}

int
main(int argc, char** argv)
{
	printf("%s\n", getcwd(NULL, 0));
	parent = getpid();

	signal(SIGINT, shutdown);

	if(argc < 2) {
		error("Usage is `cdemon command arguments...\neg.\ncdemon ls -alh`");
	}

	command = strdup(argv[1]);
	if(argc == 2) {
		args = NULL;
	} else {
		args = &argv[1];
	}

	pthread_mutex_init(&restart_lock, NULL);

	tpool_t *tpool = tpool_new(2);
	char* command = argv[2];

	tpool_push(tpool, watch, (void*) tpool);

	tpool_finish(tpool);
}