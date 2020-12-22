
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "Utils.hpp"
#include "JsonParser.hpp"
#include "msg.hpp"
#include "sem.hpp"
#include "shm.hpp"
#include "Logger.hpp"
#include "Common.hpp"
#include "mmap.hpp"

#define MMAP_SIZE                       2*1024*1024
#define NUMBER_OF_H264_FRAME_FILES      403
#define DEFAULT_FPS_VALUE               25
#define STR_LENGTH      "length"
#define STR_DATA        "data"

std::unique_ptr<Posix::Mmap<char>> MmapObj=nullptr;
SystemV::Sem *ShmSemObject=nullptr;
int thread_running=0;

ssize_t ReadDataFromFile(const char *filename, char *buffer)
{
    ssize_t read_size=0;
    struct stat filestat;
    int fd;
    if((fd=open(filename,O_RDONLY))==-1)
    {
        printf("read error");
    }
    fstat(fd,&filestat);
    read_size = read(fd,buffer,filestat.st_size);
    close(fd);
    return read_size;
}

void *WriteMmapThreadRoutine(void *arg)
{   
    int frame_count=1;
    char filename[256];
    ssize_t fileszie=0;
    useconds_t interval_time=1000000/DEFAULT_FPS_VALUE;

    while(thread_running)
    {
        ShmSemObject->Decrease(0);
        if(MmapObj!=nullptr)
        {
            bzero(filename,sizeof(filename)/sizeof(char));
            sprintf(filename,"./h264SampleFrames/frame-%03d.h264",frame_count);
            LOG_OUT(INFO,"Load %03d Frame",frame_count);

            MmapObj->Clean();
            fileszie=ReadDataFromFile(filename,MmapObj->GetMmapPtr()+sizeof(ssize_t));
            MmapObj->SetMmapSize(fileszie);
            frame_count++;
            if(frame_count>NUMBER_OF_H264_FRAME_FILES)
            {
                frame_count=1;
            }
        }
        usleep(interval_time);
        ShmSemObject->Increase(1); 
    }
    LOG_OUT(INFO,"WriteMmapThreadRoutine Exit Normally");
    pthread_exit(NULL);
} 

void sigcatch(int signo)
{
    LOG_OUT(INFO,"Signal %d Has Catched!\n",signo);
    thread_running=0;
    sleep(1);
    if(ShmSemObject)
    {
        delete ShmSemObject;
        ShmSemObject=nullptr;
    }
    sigset(signo,SIG_DFL);
    raise(signo);
}

int main(int argc,char *argv[])
{
    pthread_t MmapTid;

    CompilerInfo();

    MmapObj=Posix::Mmap<char>::Creat("./mmapfile",MMAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED);
    if(MmapObj==nullptr)
    {
        LOG_OUT(ERROR,"mmap Creat Error!\n");
        exit(EXIT_FAILURE);
    }
    LOG_OUT(INFO,"Frame Send Process Runing...\n");

    int semval[2] = {1 , 0};//创建两个信号量用来共享内存互斥读写
    ShmSemObject=new SystemV::Sem("/",5,semval,sizeof(semval)/sizeof(int));// 两个不同的进程进行信号量通信，需要保持一样的信号量值以及ipc_key

    thread_running=1;
    pthread_create(&MmapTid,NULL,WriteMmapThreadRoutine,NULL);
    pthread_detach(MmapTid);

    while(1)
    {
        sleep(1);
    }

    return 0;
}
