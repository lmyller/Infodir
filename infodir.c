#include "infodir.h"

int infodir(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Informe apenas o diretório de busca\n");
        exit(EXIT_FAILURE);
    }

    infodirProcess(argv[1]);
    infodirThread(argv[1]);

    return EXIT_SUCCESS;
}

void infodirProcess(const char *argv)
{
    Infodir *infodir;
    time_t timeStart, timeFinish;
    int duration;
    char strTimeStart[SIZE_TIME], strTimeFinish[SIZE_TIME];
    int segmentId = createSharedMemory();

    infodir = attachedSegmentMemory(segmentId);

    clearInfodir(infodir);

    timeStart = getCurrentTime();

    readRootDir(argv, segmentId, IPC);

    timeFinish = getCurrentTime();

    duration = getDuration(timeStart, timeFinish);

    convertTimeToStr(timeStart, strTimeStart);
    convertTimeToStr(timeFinish, strTimeFinish);

    showReport(argv, infodir, strTimeStart, strTimeFinish, duration, IPC);

    desconnectSharedMemory(infodir);
}

void infodirThread(const char *argv)
{
    Infodir *infodir;
    time_t timeStart, timeFinish;
    int duration;
    char strTimeStart[SIZE_TIME], strTimeFinish[SIZE_TIME];
    int segmentId = createSharedMemory();

    infodir = attachedSegmentMemory(segmentId);

    clearInfodir(infodir);

    timeStart = getCurrentTime();

    readRootDir(argv, segmentId, MULTITHREAD);

    timeFinish = getCurrentTime();

    duration = getDuration(timeStart, timeFinish);

    convertTimeToStr(timeStart, strTimeStart);
    convertTimeToStr(timeFinish, strTimeFinish);

    showReport(argv, infodir, strTimeStart, strTimeFinish, duration, MULTITHREAD);

    desconnectSharedMemory(infodir);
}

void clearInfodir(Infodir *infodir)
{
    infodir->directories = 0;
    infodir->files = 0;
    infodir->sizeDirectory = 0;
}

void readRootDir(const char *arg, int segmentId, const char *method)
{
    DIR *dir;
    struct dirent *dirent;
    char path[SIZE_PATH], fullPath[SIZE_PATH];
    int countFile = 0;

    strcpy(path, arg);
    dir = opendir(path);

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if ((strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0))
            {
                if (dirent->d_type == DT_DIR)
                {
                    concatPath(path, fullPath, dirent->d_name);

                    Arguments arguments;

                    strcpy(arguments.path, fullPath);
                    arguments.segmentId = segmentId;

                    if (strcmp(method, IPC) == 0)
                        process(arguments, dirent->d_type);

                    else
                        thread(arguments);
                }

                else if (dirent->d_type == DT_REG)
                    countFile++;
            }
        }
        closedir(dir);

        if (countFile > 0)
            readRootFiles(segmentId, arg);
    }

    else
    {
        printf("Diretório inválido\n");
        exit(EXIT_FAILURE);
    }
}

int readSubDir(void *args)
{
    Arguments arguments = *(Arguments *)args;
    DIR *dir;
    struct dirent *dirent;
    char path[SIZE_PATH], fullPath[SIZE_PATH];

    strcpy(path, arguments.path);
    dir = opendir(path);

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if ((strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0))
            {
                concatPath(path, fullPath, dirent->d_name);
                strcpy(arguments.path, fullPath);
                childProcess(dirent->d_type, arguments.segmentId, arguments.path);

                if (dirent->d_type == DT_DIR)
                    readSubDir(&arguments);
            }
        }
        closedir(dir);
    }
}

void process(Arguments arguments, const unsigned char type)
{
    pid_t pid = createProcess();

    if (pid == 0)
    {
        if (type == DT_DIR)
            childProcess(type, arguments.segmentId, arguments.path);

        readSubDir(&arguments);
        exit(EXIT_SUCCESS);
    }

    else
        wait(0);
}

void readRootFiles(const int segmentId, const char *path)
{
    pid_t pid = createProcess();

    if(pid == 0){

        DIR *dir;
        struct dirent *dirent;
        char fullPath[SIZE_PATH];

        dir = opendir(path);

        while ((dirent = readdir(dir)) != NULL)
        {
            if(dirent->d_type == DT_REG){
                concatPath(path, fullPath, dirent->d_name);
                childProcess(dirent->d_type, segmentId, fullPath);
            }
        }

        exit(EXIT_SUCCESS);
    }

    else
        wait(0);
}

void childProcess(const unsigned char type, const int segmentId, const char *fullPath)
{
    Infodir *infodir;

    infodir = attachedSegmentMemory(segmentId);

    if (type == DT_DIR)
        infodir->directories++;

    else if (type == DT_REG)
    {
        infodir->sizeDirectory += getSizeFile(fullPath);
        infodir->files++;
    }

    desconnectSharedMemory(infodir);
}

pid_t createProcess()
{
    return fork();
}

void thread(Arguments arguments)
{
    thrd_t threadId = threadChild(arguments);
    printf("%s\n", "Thread");
    thrd_join(threadId, NULL);
}

pthread_t threadChild(Arguments arguments)
{
    thrd_t threadId;
    printf("%s\n", "Thread filha");
    int status = thrd_create(&threadId, readSubDir, &arguments);
    return (status == thrd_success) ? threadId : status;
}

long getSizeFile(const char *pathFile)
{
    FILE *file = openFile(pathFile);

    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);

        return ftell(file);
    }

    return 0;
}

FILE *openFile(const char *pathFile)
{
    return fopen(pathFile, "rb");
}

int createSharedMemory()
{
    return shmget(IPC_PRIVATE, sizeof(Infodir), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
}

Infodir *attachedSegmentMemory(int segmentId)
{
    return (Infodir *)shmat(segmentId, NULL, 0);
}

void desconnectSharedMemory(Infodir *infodir)
{
    shmdt(infodir);
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

// Exibe o relatório na tela da quantidade de diretórios e arquivos pesquisados e o tempo de pesquisa
void showReport(const char *path, const Infodir *infodir, const char *timeStart, const char *timeFinish, const int duration, const char *method)
{
    printf("\n%s %s", METHOD, method);
    printf("\n%s%s\n", DIRECTORY, path);
    printf("\n%s", DIRECTORY_CONTENT);
    printf("\n%s %d", FILES, infodir->files);
    printf("\n%s %d", SUB_DIRECTORIES, infodir->directories);
    printf("\n%s %ld\n", SIZE_DIRECTORY, infodir->sizeDirectory);
    printf("\n%s%s", TIME, method);
    printf("\n%s%s", START, timeStart);
    printf("\n%s%s", FINISH, timeFinish);
    printf("\n%s%d%s\n\n", DURATION, duration, SEGUNDOS);
}

// Converte a hora em time_t para string
void convertTimeToStr(time_t hora, char *horaString)
{
    struct tm *structHora;

    structHora = localtime(&hora);

    // Passando a hora para string
    sprintf(horaString, "%02d:%02d:%02d", structHora->tm_hour, structHora->tm_min, structHora->tm_sec);
}

// Obtém a hora e data atual
time_t getCurrentTime()
{
    time_t hora;

    time(&hora);

    return hora;
}

// Obtém a diferença de dois horários
int getDuration(time_t horaInicio, time_t horaFim)
{
    return difftime(horaFim, horaInicio);
}

int main(int argc, char *argv[])
{
    return infodir(argc, argv);
}