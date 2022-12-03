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

#define SIZE_PATH 1000
#define DIRECTORY "- Diretório: "
#define METHOD "- Método:" 
#define IPC "IPC - Interprocess Communication"
#define DIRECTORY_CONTENT "- Conteúdo do diretório"
#define FILES "  Files = "
#define SUB_DIRECTORIES "  Subdiretórios = "
#define SIZE_DIRECTORY "  Tamanho do diretório = "
#define IPC_TIME "- Tempo usando IPC"
#define START "  Início.: "
#define FINISH "  Término: "
#define DURATION "  Duração: "
#define SEGUNDOS " Segundos"

typedef struct infodir
{
    unsigned int files, directories;
    long sizeDirectory;
}Infodir;

int infodir(int argc, char *argv[]);
void readRootDir(const char *arg, int segmentId);
void readSubDir(const char *arg, int segmentId);
pid_t createProcess();
void childProcess(const unsigned char type, int segmentId, char *fullPath);
long getSizeFile(char *pathFile);
FILE *openFile(char *pathFile);
int createSharedMemory();
Infodir* attachedSegmentMemory(int segmentId);
void desconnectSharedMemory(Infodir *infodir);
void concatPath(const char *path, char *fullPath, const char *nameDir);
void cleanString(char *str, const unsigned int sizeString);

void showReport(char *path, Infodir *infodir, char *timeStart, char *timeFinish, int duration, const char *method);

//Converte a hora em time_t para string
void convertTimeToStr(time_t hora, char *horaString);

//Converte a data em time_t para string
void convertTimeToStr(time_t data, char *dataString);

//Obtém a hora e data atual
time_t getCurrentTime();

//Obtém a diferença de dois horários
int getDuration(time_t horaInicio, time_t horaFim);

#endif