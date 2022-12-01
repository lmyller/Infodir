#include "infodir.h"

int infodir(int argc, char *argv[])
{
    Infodir infodir;

    listDirAndFiles(argv[1], &infodir);

    printf("\nDiretorios: %d\t\t\tFiles: %d\n", infodir.directories, infodir.files);

    return EXIT_SUCCESS;
}

void process(Infodir *infodir){
    Infodir *ptr;

    int fd = createSharedMemory();

    setSizeSharedMemory(&fd, sizeof(infodir));

    ptr = mapSharedMemory(fd, sizeof(Infodir));

    printf("Dir: %d\t\t\tFiles: %d", infodir->directories, infodir->files);

    infodir->directories += ptr->directories;
    infodir->files += ptr->files;
}

void createProcess(const unsigned char type, Infodir *infodir)
{
    pid_t pid = fork();
    

    if (pid != 0){
        printf("Pid filho: %d\n", getpid());
    //    childProcess(type);
    }
    else{
        wait(NULL);
        printf("Pid pai: %d\n", getpid());
    }
}

void listDirAndFiles(const char *arg, Infodir *infodir)
{
    DIR *dir;
    struct dirent *dirent;
    char path[SIZE_PATH], fullPath[SIZE_PATH];

    strcpy(path, arg);
    dir = opendir(arg);

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if ((strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0))
            {
                createProcess(dirent->d_type, infodir);

                if (dirent->d_type == DT_DIR)
                {
                    concatPath(path, fullPath, dirent->d_name);

                    listDirAndFiles(fullPath, infodir);
                }
            }
        }
        closedir(dir);
    }
}

void childProcess(const unsigned char type)
{
    Infodir infodir, *ptr;

    ptr->directories = 0;
    ptr->files = 0;

    int fd = createSharedMemory();

    setSizeSharedMemory(&fd, sizeof(infodir));

    ptr = mapSharedMemory(fd, sizeof(Infodir));

    if (type == DT_DIR)
        infodir.directories++;

    else
        infodir.files++;

    (*ptr) = infodir;

    exit(1);
}

int createSharedMemory()
{
    int fd = shm_open("/sharedmem", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    return fd;
}

void setSizeSharedMemory(int *fd, const size_t size)
{
    if (ftruncate(*fd, size) == -1)
    {
        perror("ftruncate");
        exit(1);
    }
}

Infodir* mapSharedMemory(const int fd, const size_t size)
{
    Infodir *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    return ptr;
}

void concatPath(const char *path, char *fullPath, const char *nameDir)
{
    cleanString(fullPath, SIZE_PATH);

    strcpy(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, nameDir);
}

void cleanString(char *str, const unsigned int sizeString)
{
    memset(str, '\0', sizeString);
}

int main(int argc, char *argv[])
{
    return infodir(argc, argv);
}