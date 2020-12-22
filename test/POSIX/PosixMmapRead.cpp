
#include <signal.h>
#include <memory>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#include "Logger.hpp"
#include "Common.hpp"
#include "mmap.hpp"
#include "sem.hpp"
#include "Utils.hpp"
#include "JsonParser.hpp"

#define MMAP_SIZE 200*1024

std::unique_ptr<Posix::Mmap<char>> MmapObj=nullptr;
SystemV::Sem *ShmSemObject=nullptr;
int thread_running=0;

void *ReadMmapThreadRoutine(void *arg)
{
    std::string buffer;    
    while(thread_running)
    {
        ShmSemObject->Decrease(1);
        if(MmapObj!=nullptr)
        {
            LOG_OUT(INFO,"Mmap Data:\n%s\n",MmapObj->GetMmapPtr());
        }
        ShmSemObject->Increase(0);
        sleep(1);
    }
    LOG_OUT(INFO,"ReadMmapThreadRoutine Exit Normally");
    pthread_exit(NULL);
} 

void *WriteMmapThreadRoutine(void *arg)
{   
    while(thread_running)
    {
        ShmSemObject->Decrease(0);
        if(MmapObj!=nullptr)
        {
            Utils::JsonParser::JsonDocument JsonDoc;
            JsonDoc.SetObject();
            JsonDoc.AddMember("Version","2.0.3",JsonDoc.GetAllocator());
            JsonDoc.AddMember("Temperature","32",JsonDoc.GetAllocator());
            JsonDoc.AddMember("Alarm",false,JsonDoc.GetAllocator());
            JsonDoc.AddMember("Read",rapidjson::kArrayType,JsonDoc.GetAllocator());
            JsonDoc["Read"].PushBack(1,JsonDoc.GetAllocator());
            JsonDoc["Read"].PushBack(2,JsonDoc.GetAllocator());
            JsonDoc["Read"].PushBack(3,JsonDoc.GetAllocator());
            MmapObj->Wirte(Utils::JsonParser::ToString(JsonDoc).c_str(),0,strlen(Utils::JsonParser::ToString(JsonDoc).c_str()));
        }
        ShmSemObject->Increase(1);
        sleep(2);
    }
    LOG_OUT(INFO,"WriteMmapThreadRoutine Exit Normally");
    pthread_exit(NULL);
} 

void sigcatch(int signo)
{
    LOG_OUT(INFO,"Signal %d Has Catched!\n",signo);
    thread_running=0;
    sleep(1);
    sigset(signo,SIG_DFL);
    raise(signo);
}

int main(int argc,char *argv[])
{
    pthread_t MmapTid;

    MmapObj=Posix::Mmap<char>::Creat("./mmap_file",MMAP_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED);
    if(MmapObj==nullptr)
    {
        LOG_OUT(ERROR,"mmap Creat Error!\n");
        exit(EXIT_FAILURE);
    }

    int semval[2] = {1 , 0};//创建两个信号量用来共享内存互斥读写
    ShmSemObject=new SystemV::Sem("/",5,semval,sizeof(semval)/sizeof(int));// 两个不同的进程进行信号量通信，需要保持一样的信号量值以及ipc_key

    thread_running=1;
    pthread_create(&MmapTid,NULL,ReadMmapThreadRoutine,NULL);
    pthread_detach(MmapTid);

    while(1)
    {
        sleep(1);
    }

    return 0;
}
