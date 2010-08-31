#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define ERR(X) fprintf(stderr, "%s", (X));
const static int SLEEP_TIME = 5;
#define DEBUG

struct src_to_dest {
	char *from;
	char *to;
	mode_t access_mode;
};

src_to_dest *make_std(char *from, char *to, mode_t access_mode);
void *cp_dir(void *p);
void *cp_file(void *p);
void free_std(src_to_dest *std);
void *cp(void *p);

void *cp_dir(void *p) {

	src_to_dest *std = (src_to_dest*) p;

	if (mkdir(std->to, std->access_mode) == -1) {
		ERR("\nmkdir: ")
			ERR(std->to)
			return NULL;
	}

	DIR *dir;

mark:
	dir = opendir(std->from);

	if (dir == NULL) {

		if (errno == EMFILE)
			goto mark;

		ERR("\nopendir: ")
			ERR(std->from)
			free_std(std);
		return NULL;
	}

	struct dirent *result;
	struct dirent *entry = (struct dirent*) new char[offsetof(struct dirent, d_name) +
		pathconf(std->from, _PC_NAME_MAX) + 1];

	while (!readdir_r(dir, entry, &result)) {
		if (result == NULL) {
			break;
		}

		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		// + 1 for / and another for '\0'
		char new_from[strlen(std->from) + strlen(entry->d_name) + 1 + 1];
		char new_to[strlen(std->to) + strlen(entry->d_name) + 1 + 1];
		sprintf(new_from, "%s/%s", std->from, entry->d_name);
		sprintf(new_to, "%s/%s", std->to, entry->d_name);


		src_to_dest *new_std = make_std(new_from, new_to, 0);
		cp(new_std);
		free_std(new_std);
	}

	delete[] entry;
	closedir(dir);

	// error occured
	if (result != NULL) {
		ERR("\nreaddir")
	}

	free_std(std);
	return NULL;
}

void *cp_file(void *p) {
start:
	src_to_dest *std = (src_to_dest*) p;

	// file descriptors
	int from_d, to_d;

	from_d = open(std->from, O_RDONLY);
	if (from_d == -1) {
		if (errno == EMFILE) {
			sleep(SLEEP_TIME);
			goto start;
		}
		ERR("\nopen src file: ")
			ERR(std->from)
			free_std(std);
		return NULL;
	}

	to_d = open(std->to, O_CREAT | O_WRONLY, std->access_mode);
	if (to_d == -1) {
		close(from_d);
		if (errno == EMFILE) {
			sleep(SLEEP_TIME);
			goto start;
		}

		ERR("\nopen dst file: ")
			ERR(std->to)
			free_std(std);
		return NULL;
	}

	char buf[BUFSIZ];
	int bytes_read;

	while (bytes_read = read(from_d, buf, sizeof (buf))) {
		if (write(to_d, buf, bytes_read) != bytes_read) {
			ERR("\nwrite dst file: ")
				ERR(std->to)
				break;
		}
	}

	close(from_d);
	close(to_d);
	free_std(std);

	return NULL;
}

void *cp(void *p) {
	struct stat stat;
	lstat(((src_to_dest*) p)->from, &stat);
	if (S_ISREG(stat.st_mode) || S_ISDIR(stat.st_mode)) {
		src_to_dest *std = make_std(((src_to_dest*) p)->from, ((src_to_dest*) p)->to, stat.st_mode);
		pthread_t tid;

		if (S_ISREG(stat.st_mode)) {
			if (pthread_create(&tid, NULL, cp_file, std) != 0) {
				ERR("cp_file")
			} else {
				pthread_detach(tid);
			}
		} else if (S_ISDIR(stat.st_mode)) {
			if (pthread_create(&tid, NULL, cp_dir, std) != 0) {
				ERR("cp_dir")
			} else {
				pthread_detach(tid);
			}
		}
	}
	return NULL;
}

src_to_dest *make_std(char *from, char *to, mode_t access_mode) {
	src_to_dest *std = new src_to_dest;

	int from_len = strlen(from);
	int to_len = strlen(to);

	std->from = new char[from_len + 1];
	std->to = new char[to_len + 1];

	strcpy(std->from, from);
	strcpy(std->to, to);

	std->access_mode = access_mode;
#ifdef DEBUG
	fprintf(stderr, "\nFrom:\t%s\nTo:\t%s\n\n", std->from, std->to);
#endif
	return std;
}

void free_std(src_to_dest *std) {
	delete[] std->from;
	delete[] std->to;
	delete std;
}


int main(int argc, char *argv[]) {
#ifdef DEBUG
	for(int i = 0; i < argc; ++i){
		printf("argv[%d] = \"%s\"\n", i, argv[i]);
		fflush(stdout);
	}
#endif
	if (argc != 3) {
		fprintf(stderr, "Usage: %s src_dir dst_dir\n", argv[0]);
		exit(1);
	}

	struct stat stat;
	lstat(argv[1], &stat);

	char to[strlen(argv[2]) + strlen(basename(argv[1])) + 1 + 1];
	sprintf(to, "%s/%s", argv[2], basename(argv[1]));
	src_to_dest *std = make_std(argv[1], to, stat.st_mode);

	cp(std);

	free_std(std);
	pthread_exit(NULL);
}
