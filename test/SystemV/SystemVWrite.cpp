
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
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

#define SHM_SIZE 200*1024
int thread_running=0;

SystemV::Msg *RecvMsgObject;
SystemV::Msg *SendMsgObject;
SystemV::Shm *ShmObject;
SystemV::Sem *ShmSemObject;

/* 在实际项目中用做进程间通信，这里是为了方便测试 */
void *RecvMsgThreadRoutine(void *arg)
{
    std::string buffer;   
    while(thread_running)
    {
        if(RecvMsgObject->Receive(0,buffer,true)!=-1) //接收任意消息类型
        {
            LOG_OUT(INFO,"Massage Queue Receive Data[%ld]\n%s\n",RecvMsgObject->MsgType(),buffer.c_str());
            SendMsgObject->Send(RecvMsgObject->MsgType(),buffer,false);
            printf("Massage Queue Send Data [%ld]\n%s\n",SendMsgObject->MsgType(),buffer.c_str());
        }
    }
}   

/* 在实际项目中用做进程间通信，这里是为了方便测试  */
void *SendMsgThreadRoutine(void *arg)
{
    std::string buffer;
    int msgtype;

    Utils::JsonParser::JsonDocument JsonDoc;
    JsonDoc.SetObject();
    JsonDoc.AddMember("Version","2.0.3",JsonDoc.GetAllocator());
    JsonDoc.AddMember("Temperature","32",JsonDoc.GetAllocator());
    JsonDoc.AddMember("Alarm",false,JsonDoc.GetAllocator());
    JsonDoc.AddMember("Read",rapidjson::kArrayType,JsonDoc.GetAllocator());
    JsonDoc["Read"].PushBack(1,JsonDoc.GetAllocator());
    JsonDoc["Read"].PushBack(2,JsonDoc.GetAllocator());
    JsonDoc["Read"].PushBack(3,JsonDoc.GetAllocator());

    while(thread_running)
    {
        msgtype++;
        buffer=Utils::JsonParser::ToString(JsonDoc);
        SendMsgObject->Send(msgtype,buffer,false);
        printf("Massage Queue Send Data [%ld]\n%s\n",SendMsgObject->MsgType(),buffer.c_str());
        sleep(2);
    }
}

/* 在实际项目中用做进程间通信，这里是为了方便测试  */
void *ReadShmThreadRoutine(void *arg)
{
    char readbuffer[SHM_SIZE];
    while(thread_running)
    {
        bzero(readbuffer,sizeof(readbuffer)/sizeof(char));
        ShmSemObject->Decrease(1);   // P操作锁定临界资源
        ShmObject->Receive(readbuffer,0,SHM_SIZE);
        ShmSemObject->Increase(0);   // V操作解锁临界资源

        //每隔1秒读取并处理共享内存数据
        LOG_OUT(INFO,"Shm Receive Data:\n%s\n",readbuffer);
        sleep(1);
    }
}

/* 在实际项目中用做进程间通信，这里是为了方便测试  */
void *WriteShmThreadRoutine(void *arg)
{
    int count=0;
    char writebuffer[SHM_SIZE];
    while(thread_running)
    {
        bzero(writebuffer,sizeof(writebuffer)/sizeof(char));

        Utils::JsonParser::JsonDocument JsonDoc;
        JsonDoc.SetObject();
        JsonDoc.AddMember("Count",count++,JsonDoc.GetAllocator());
        memcpy(writebuffer,
                Utils::JsonParser::ToString(JsonDoc).c_str(),
                Utils::JsonParser::ToString(JsonDoc).size());

        ShmSemObject->Decrease(0);   // P操作锁定临界资源
        ShmObject->Send(writebuffer,0,SHM_SIZE);
        ShmSemObject->Increase(1);   // V操作解锁临界资源

        LOG_OUT(INFO,"Shm Send Data:\n%s\n",Utils::JsonParser::ToString(JsonDoc).c_str());
        //每隔2秒向共享内存写入数据
        sleep(2);
    }
}

void sigcatch(int signo)
{
    LOG_OUT(INFO,"Signal %d Has Catched!\n",signo);

    thread_running=0;
    sleep(1);

    RecvMsgObject->~Msg();
    ShmObject->~Shm();
    ShmSemObject->~Sem();

    sigset(signo,SIG_DFL);
    raise(signo);
}

int main(int argc,char *argv[])
{
    RecvMsgObject=new SystemV::Msg("/",1);  // 两个不同的进程进行消息队列通信，需要保持一样的ipc_key
    SendMsgObject=new SystemV::Msg("/",2);  // 两个不同的进程进行消息队列通信，需要保持一样的ipc_key
    ShmObject=new SystemV::Shm("/",3,SHM_SIZE);// 两个不同的进程进行共享内存通信，需要保持一样的共享内存大小以及ipc_key
    int semval[2] = {1 , 0};//创建两个信号量用来共享内存互斥读写
    ShmSemObject=new SystemV::Sem("/",4,semval,sizeof(semval)/sizeof(int));// 两个不同的进程进行信号量通信，需要保持一样的信号量值以及ipc_key

    // Ctrl+C信号捕捉销毁资源
    struct sigaction newsig;
    newsig.sa_flags=0;
    sigemptyset(&newsig.sa_mask);
    newsig.sa_handler=sigcatch;
    sigaction(SIGINT,&newsig,NULL);

    thread_running=1;

    pthread_t RecvMsgTid;
    pthread_create(&RecvMsgTid,NULL,RecvMsgThreadRoutine,NULL);
    pthread_detach(RecvMsgTid);

    // pthread_t SendMsgTid;
    // pthread_create(&SendMsgTid,NULL,SendMsgThreadRoutine,NULL);
    // pthread_detach(SendMsgTid);

    pthread_t WriteShmTid;
    pthread_create(&WriteShmTid,NULL,WriteShmThreadRoutine,NULL);
    pthread_detach(WriteShmTid);

    while(1)
    {
        sleep(1);
    }

    return 0;
}
