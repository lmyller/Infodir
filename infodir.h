#ifndef INFODIR_H_INCLUDED
#define INFODIR_H_INCLUDED

#include <stdio.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <threads.h>

#define SIZE_PATH 1000
#define SIZE_TIME 9
#define DIRECTORY "- Diretório: "
#define METHOD "- Método:" 
#define IPC "IPC - Interprocess Communication"
#define MULTITHREAD "Multithread"
#define DIRECTORY_CONTENT "- Conteúdo do diretório"
#define FILES "  Files = "
#define SUB_DIRECTORIES "  Subdiretórios = "
#define SIZE_DIRECTORY "  Tamanho do diretório = "
#define TIME "- Tempo usando "
#define START "  Início.: "
#define FINISH "  Término: "
#define DURATION "  Duração: "
#define SEGUNDOS " Segundos"

typedef struct
{
    unsigned int files, directories;
    long sizeDirectory;
}Infodir;

typedef struct {
    char path[SIZE_PATH];
    int segmentId;
}Arguments;

int infodir(int argc, char *argv[]);
void infodirProcess(const char *argv);
void infodirThread(const char *argv);
void clearInfodir(Infodir *infodir);
void readRootDir(const char *arg, int segmentId, const char *method);
int readSubDir(void *args);
void process(Arguments arguments, const unsigned char type);
void readRootFiles(const int segmentId, const char *fullPath);
void childProcess(const unsigned char type, const int segmentId, const char *fullPath);
pid_t createProcess();
void thread(Arguments arguments);
pthread_t threadChild(Arguments arguments);
long getSizeFile(const char *pathFile);
FILE *openFile(const char *pathFile);
int createSharedMemory();
Infodir* attachedSegmentMemory(int segmentId);
void desconnectSharedMemory(Infodir *infodir);
void concatPath(const char *path, char *fullPath, const char *nameDir);
void cleanString(char *str, const unsigned int sizeString);

void showReport(const char *path, const Infodir *infodir, const char *timeStart, const char *timeFinish, const int duration, const char *method);

//Converte a hora em time_t para string
void convertTimeToStr(time_t hora, char *horaString);

//Converte a data em time_t para string
void convertTimeToStr(time_t data, char *dataString);

//Obtém a hora e data atual
time_t getCurrentTime();

//Obtém a diferença de dois horários
int getDuration(time_t horaInicio, time_t horaFim);

#endif