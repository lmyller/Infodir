#ifndef INFODIR_H_INCLUDED
#define INFODIR_H_INCLUDED

#include <stdio.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#define SIZE_PATH 1000

typedef struct infodir
{
    unsigned int files, directories;
}Infodir;

int infodir(int argc, char *argv[]);
void listDirAndFiles(const char *arg, Infodir *infodir);
//void createProcess(const unsigned char type, Infodir *infodir);
void childProcess(const unsigned char type);
int createSharedMemory();
void setSizeSharedMemory(int *fd, const size_t size);
Infodir* mapSharedMemory(const int fd, const size_t size);
void concatPath(const char *path, char *fullPath, const char *nameDir);
void cleanString(char *str, const unsigned int sizeString);

#endif