#include "infodir.h"

void listDirAndFiles(char *arg, Infodir *infodir)
{
    DIR *dir;
    struct dirent *dirent;
    char path[1000], fullPath[1000];

    strcpy(path, arg);
    dir = opendir(arg);

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if ((strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0))
            {
                if (dirent->d_type == DT_DIR)
                {
                    infodir->directories++;

                    memset(fullPath, '\0', 1000);
                    strcpy(fullPath, path);
                    strcat(fullPath, "/");
                    strcat(fullPath, dirent->d_name);

                    listDirAndFiles(fullPath, infodir);
                }

                else
                    infodir->files++;
            }
        }
        closedir(dir);
    }
}

int infodir(int argc, char *argv[])
{
    Infodir infodir;

    listDirAndFiles(argv[1], &infodir);

    printf("\nDiretorios: %d\t\t\tFiles: %d\n", infodir.directories, infodir.files);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    return infodir(argc, argv);
}