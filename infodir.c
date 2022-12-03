#include "infodir.h"

int infodir(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Informe apenas o diretório de busca\n");
        exit(EXIT_FAILURE);
    }

    Infodir *infodir;
    time_t timeStart, timeFinish;
    int duration;
    char strTimeStart[9], strTimeFinish[9];
    int segmentId = createSharedMemory();

    infodir = attachedSegmentMemory(segmentId);

    infodir->directories = 0;
    infodir->files = 0;
    infodir->sizeDirectory = 0;

    timeStart = getCurrentTime();

    readRootDir(argv[1], segmentId);

    timeFinish = getCurrentTime();

    duration = getDuration(timeStart, timeFinish);

    convertTimeToStr(timeStart, strTimeStart);
    convertTimeToStr(timeFinish, strTimeFinish);

    showReport(argv[1], infodir, strTimeStart, strTimeFinish, duration, IPC);

    desconnectSharedMemory(infodir);

    return EXIT_SUCCESS;
}

pid_t createProcess()
{
    return fork();
}

void readRootDir(const char *arg, int segmentId)
{
    DIR *dir;
    struct dirent *dirent;
    char path[SIZE_PATH], fullPath[SIZE_PATH];

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
                    pid_t pid = createProcess();

                    concatPath(path, fullPath, dirent->d_name);

                    if (pid == 0)
                    {
                        childProcess(dirent->d_type, segmentId, fullPath);
                        readSubDir(fullPath, segmentId);
                    }

                    else
                    {
                        wait(0);
                        exit(EXIT_SUCCESS);
                    }
                }

                /*else if (dirent->d_type == DT_REG)
                {
                    pid_t pid = createProcess(){

                    }
                }*/
            }
        }
        closedir(dir);
    }

    else
    {
        printf("Diretório inválido\n");
        exit(EXIT_FAILURE);
    }
}

void readSubDir(const char *arg, int segmentId)
{
    DIR *dir;
    struct dirent *dirent;
    char path[SIZE_PATH], fullPath[SIZE_PATH];

    strcpy(path, arg);
    dir = opendir(path);

    if (dir)
    {
        while ((dirent = readdir(dir)) != NULL)
        {
            if ((strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0))
            {
                concatPath(path, fullPath, dirent->d_name);
                childProcess(dirent->d_type, segmentId, fullPath);

                if (dirent->d_type == DT_DIR)
                    readSubDir(fullPath, segmentId);
            }
        }
        closedir(dir);
    }
}

void childProcess(const unsigned char type, int segmentId, char *fullPath)
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

long getSizeFile(char *pathFile)
{
    FILE *file = openFile(pathFile);

    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);

        return ftell(file);
    }
    
    return 0;
}

FILE *openFile(char *pathFile)
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
void showReport(char *path, Infodir *infodir, char *timeStart, char *timeFinish, int duration,
                const char *method)
{
    printf("\n%s %s", METHOD, method);
    printf("\n%s%s\n", DIRECTORY, path);
    printf("\n%s", DIRECTORY_CONTENT);
    printf("\n%s %d", FILES, infodir->files);
    printf("\n%s %d", SUB_DIRECTORIES, infodir->directories);
    printf("\n%s %ld\n", SIZE_DIRECTORY, infodir->sizeDirectory);
    printf("\n%s", IPC_TIME);
    printf("\n%s%s", START, timeStart);
    printf("\n%s%s", FINISH, timeFinish);
    printf("\n%s%d%s\n", DURATION, duration, SEGUNDOS);
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