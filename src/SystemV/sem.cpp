#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sem.hpp"

namespace SystemV
{
    Sem::Sem(const char* path, int proj_id, int *val, int semnum)
    {
        m_sem_key=ftok(path, proj_id);
        if(m_sem_key==-1)
        {
            printf("Error: ftok\n");
        }

        m_sem_id = semget(m_sem_key,semnum,IPC_CREAT|IPC_EXCL|0666);
        if(m_sem_id==-1)
        {
            if(errno==EEXIST)
            {
                m_sem_id = semget(m_sem_key,0,IPC_CREAT|0666);
                if(m_sem_id==-1)
                {
                    printf("Error: semget, errno:%d, errno info:%s\n",errno, strerror(errno));
                }
            }
            else
            {
                printf("Error: semget, errno:%d, errno info:%s\n",errno, strerror(errno));
            }            
        }

        union semun sem_union;
        for(int i=0;i<semnum;i++)
        {
            sem_union.val=val[i];
            if (semctl(m_sem_id,i,SETVAL,sem_union)==-1)
            {
                printf("Error: semctl, errno:%d, errno info:%s\n",errno, strerror(errno));
            }
        }
        m_semnums=semnum;
    }

    Sem::~Sem()
    {
        // 注释掉这里避免进程报错
        // union semun sem_union;
        // for(int i=0;i<m_semnums;i++)
        // {
        //     semctl(m_sem_id,i,IPC_RMID,sem_union);
        // }       
    }

    void Sem::Delete(int semno)
    {
        union semun sem_union;
        if (semctl(m_sem_id,semno,IPC_RMID,sem_union)==-1)
        {
            printf("Error: semctl, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
    }

    void Sem::Increase(int semno)
    {
        struct sembuf sem_t;
        sem_t.sem_num=semno;
        sem_t.sem_op=1;
        sem_t.sem_flg=SEM_UNDO;

        if(semop(m_sem_id,&sem_t,1)==-1)
        {
            printf("Error: semop, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
    }

    void Sem::Decrease(int semno)
    {
        struct sembuf sem_t;
        sem_t.sem_num=semno;
        sem_t.sem_op=-1;
        sem_t.sem_flg=SEM_UNDO;

        if(semop(m_sem_id,&sem_t,1)==-1)
        {
            printf("Error: semop, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
    }

    void Sem::Info(void)
    {
        printf("\n============== Shm Info ==============\n");
        printf(" shm_key    : %d, %#x\n",m_sem_key, m_sem_key);
        printf(" shm_id     : %d, %#x\n",m_sem_id, m_sem_id);
        printf("======================================\n");
    }
}