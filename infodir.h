#ifndef INFODIR_H_INCLUDED
#define INFODIR_H_INCLUDED

#include <stdio.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

typedef struct infodir
{
    unsigned int files, directories;
}Infodir;


#endif