#include <include/watch.h>
#include <include/util.h>
#include <include/global.h>
#include <include/run.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <signal.h>
#include <libtpool/tpool.h>
#include <libvector/vector.h>
#include <pthread.h>
#include <sys/stat.h>
#include <libhmap/hmap.h>
#include <dirent.h>

#include <stdio.h>
#include <errno.h>

#define BUF_LEN 4096
#define INT_TIMER 500000

int notify_fd;
vector_t *notify_wds;
hmap_t *dir_wd;

char* get_dir_path(const char* name, int parent_wd) {

	char *parent_wd_str = malloc(10 * sizeof(char));
	sprintf(parent_wd_str,"%d", parent_wd);

	char* parent_path = hmap_get(dir_wd, parent_wd_str);
	free(parent_wd_str);
	int len = strlen(parent_path) + strlen(name) + 2;

	char* path = malloc(sizeof(char) * len);
	sprintf(path, "%s/%s", parent_path, name);

	struct stat path_stat;
	stat(path, &path_stat);

	if(S_ISDIR(path_stat.st_mode)) {
		return path;
	} else {
		free(path);
		return NULL;
	}
}

void watch_dir_r(char* path) {
	int *wd = malloc(sizeof(int));
	*wd = inotify_add_watch(notify_fd, path,
		IN_CREATE | IN_MODIFY | IN_MOVE | IN_DELETE |
		IN_MOVE_SELF | IN_DELETE_SELF);

	struct stat item;
	stat(path, &item);

	char *wd_str = malloc(10 * sizeof(char));
	int k = sprintf(wd_str, "%d", *wd);
	wd_str = realloc(wd_str, k * sizeof(char));

	hmap_push(dir_wd, wd_str, (void*) path);
	vec_push_back(notify_wds, (void*) wd);

	// Get sub-directories
	printf("-> %s\n", path);
	DIR* dir = opendir(path);
	if(!dir) {
		error("Can not open directory.");
	}
	struct dirent *dp;
	while(dp = readdir(dir)) {
		if(dp->d_type != DT_DIR) {
			continue;
		}

		if(dp->d_name[0] == '.') {
			continue;
		}

		char *sub_path = get_dir_path(dp->d_name, *wd);
		if(!sub_path) {
			continue;
		}
		watch_dir_r(sub_path);
	}
}

void* watch(void* args) {

	char* path = getcwd(NULL, 0);
	notify_wds = vec_new();
	dir_wd = hmap_new();

	notify_fd = inotify_init1(IN_NONBLOCK); // instead of fcntl for non-blocking

	// Initial File Descriptor of root directory
	watch_dir_r(path);

	success("[cdemon] Starting instance...");
	run();

	int EVENT_SIZE = sizeof(struct inotify_event);
	const struct inotify_event *event;
	while(1) {
		usleep(INT_TIMER);
		char buf[BUF_LEN] = {0};

		int len = read(notify_fd, buf, BUF_LEN);
		if(len == -1) {
			continue; // If no events in buffer
		}

		success("[cdemon] Restarting instance...");
		run();

		pthread_mutex_lock(&restart_lock);
		for(char* ptr = buf; ptr < buf + len; ptr+= EVENT_SIZE + event->len) {
			event = (const struct inotify_event*) ptr;

			int n = vec_size(notify_wds);
			for(int i = 0; i<n; i++) {
				int* elem = vec_get(notify_wds, i);

				if(!elem || *elem != event->wd) {
					continue;
				}

				char* item_path = get_dir_path(event->name, event->wd);

				char *wd_str = malloc(10 * sizeof(char));
				sprintf(wd_str, "%d", event->wd);

				if(item_path && (event->mask & IN_DELETE)) {

					// TODO: Remove feature in vector
					elem = NULL;
					hmap_remove(dir_wd, wd_str);
					inotify_rm_watch(notify_fd, event->wd);

				} else if(item_path) {
					watch_dir_r(item_path);
				}

				free(wd_str);
			}
		}

		restart = true;
		pthread_mutex_unlock(&restart_lock);
	}


	char buf[4096] = {0};
}