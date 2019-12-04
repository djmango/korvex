#include <stddef.h>
#include <sys/stat.h>
// ignore this file, its a workaround for a bug

int mkdir (const char *_path, mode_t __mode ) {
	return 0;
}

int _unlink(char* name) {
	return 0;
}

int _link(char* old, char* new) {
	return 0;
}

int stat(const char* restrict path, struct stat* st) {
	return 0;
}

int chdir(const char* path) {
	return 0;
}

int chmod(const char* path, mode_t mode) {
	return 0;
}

int pathconf(const char* path, int name) {
	return 0;
}

int getcwd(char* buf, size_t size) {
	return 0;
}