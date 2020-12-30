
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

#define SHM_SIZE                        200*1024
#define MMAP_SIZE                       2*1024*1024
#define NUMBER_OF_H264_FRAME_FILES      403
#define DEFAULT_FPS_VALUE               25

bool LogOutEnable=false;

std::unique_ptr<Posix::Mmap<char>> MmapObj=nullptr;
SystemV::Sem *MampSemObject=nullptr;
SystemV::Shm *ShmObject=nullptr;
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
        MampSemObject->Decrease(0);
        if(MmapObj!=nullptr)
        {
            bzero(filename,sizeof(filename)/sizeof(char));
            sprintf(filename,"./h264SampleFrames/frame-%03d.h264",frame_count);
            if(LogOutEnable)
            {
                LOG_OUT(INFO,"Load %03d Frame",frame_count);
            }

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
        MampSemObject->Increase(1); 
    }
    LOG_OUT(INFO,"WriteMmapThreadRoutine Exit Normally");
    pthread_exit(NULL);
} 

void *WriteShmThreadRoutine(void *arg)
{
    int count=0;
    char writebuffer[SHM_SIZE];
    while(thread_running)
    {
        bzero(writebuffer,sizeof(writebuffer)/sizeof(char));

        // Utils::JsonParser::JsonDocument JsonDoc;
        // JsonDoc.SetObject();
        // JsonDoc.AddMember("Count",count++,JsonDoc.GetAllocator());
        // memcpy(writebuffer,
        //         Utils::JsonParser::ToString(JsonDoc).c_str(),
        //         Utils::JsonParser::ToString(JsonDoc).size());

        ShmSemObject->Decrease(0);   // P操作锁定临界资源
        // ShmObject->Send(writebuffer,0,SHM_SIZE);
        ShmSemObject->Increase(1);   // V操作解锁临界资源

        // LOG_OUT(INFO,"Shm Send Data:\n%s\n",Utils::JsonParser::ToString(JsonDoc).c_str());
        //每隔2秒向共享内存写入数据
        sleep(2);
    }
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

    if(argc==2)
    {
        if(strcmp(argv[1],"ON")==0||strcmp(argv[1],"On")==0||strcmp(argv[1],"on")==0||strcmp(argv[1],"oN")==0)
        {
            LogOutEnable=true;
        }
    }

    ShmObject=new SystemV::Shm("/",3,SHM_SIZE);
    int shmsemval[2] = {1 , 0};
    ShmSemObject=new SystemV::Sem("/",4,shmsemval,sizeof(shmsemval)/sizeof(int));

    MmapObj=Posix::Mmap<char>::Creat("/tmp/YgAppMmapFile",MMAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED);
    if(MmapObj==nullptr)
    {
        LOG_OUT(ERROR,"mmap Creat Error!\n");
        exit(EXIT_FAILURE);
    }
    LOG_OUT(INFO,"CubeCode Process Runing...\n");

    int mampsemval[2] = {1 , 0};//创建两个信号量用来共享内存互斥读写
    MampSemObject=new SystemV::Sem("/",5,mampsemval,sizeof(mampsemval)/sizeof(int));

    thread_running=1;
    pthread_create(&MmapTid,NULL,WriteMmapThreadRoutine,NULL);
    pthread_detach(MmapTid);

    pthread_t WriteShmTid;
    pthread_create(&WriteShmTid,NULL,WriteShmThreadRoutine,NULL);
    pthread_detach(WriteShmTid);

    while(1)
    {
        sleep(1);
    }

    return 0;
}
