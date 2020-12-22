#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shm.hpp"

namespace SystemV
{
    Shm::Shm(const char* path, int proj_id, size_t size)
    {
        m_shm_key=ftok(path,proj_id);
        if(m_shm_key==-1)
        {
            printf("Error: ftok\n");
        }

        m_shm_id = shmget(m_shm_key,size,IPC_CREAT|IPC_EXCL|0666);
        if(m_shm_id==-1)
        {
            if(errno==EEXIST)
            {
                m_shm_id = shmget(m_shm_key,0,IPC_CREAT|0666);
                if(m_shm_id==-1)
                {
                    printf("Error: shmget, errno:%d, errno info:%s\n",errno, strerror(errno));
                }
            }
            else
            {
                printf("Error: shmget, errno:%d, errno info:%s\n",errno, strerror(errno));
            }            
        }
        
        m_shm_addr = (char *)shmat(m_shm_id,NULL,0);
        if(m_shm_addr==(void *)-1)
        {
            printf("Error: shmat, errno:%d, errno info:%s\n",errno, strerror(errno));
        }

        m_size=size;
    }

    Shm::~Shm()
    {
        // 注释掉这里避免进程报错
        // struct shmid_ds shmid;
        // if (shmdt(m_shm_addr) == -1)
        // {
        //     printf("Error: shmdt, errno:%d, errno info:%s\n",errno, strerror(errno));
        // }
        // if(shmctl(m_shm_id, IPC_RMID, &shmid) == -1)
        // {
        //     printf("Error: shmctl, errno:%d, errno info:%s\n",errno, strerror(errno));
        // }
        // m_shm_addr=NULL;
    }

    void Shm::Release(void)
    {
        struct shmid_ds shmid;
        if (shmdt(m_shm_addr) == -1)
        {
            printf("Error: shmdt, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
        if(shmctl(m_shm_id, IPC_RMID, &shmid) == -1)
        {
            printf("Error: shmctl, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
        m_shm_addr=NULL;
    }

    bool Shm::Receive(char *buffer, int start_pos, size_t length)
    {
        if(start_pos<0 || (start_pos+length)>m_size)
        {
            return false;
        }
        memcpy(buffer,&m_shm_addr[start_pos],length);
        return true;
    }

    bool Shm::Send(const char *buffer, int start_pos, size_t length)
    {
        if(start_pos<0 || (start_pos+length)>m_size)
        {
            return false;
        }
        memcpy(&m_shm_addr[start_pos],buffer,length);
        return true;
    }

    void Shm::Clear(void)
    {
        if(m_shm_addr==NULL)
        {
            return;
        }
        memset(m_shm_addr, 0, m_size);
    }

    void Shm::Info(void)
    {
        struct shmid_ds shm_data;
        if(shmctl(m_shm_id,IPC_STAT,&shm_data)==-1)
        {
            printf("Error: shmctl, errno:%d, errno info:%s\n",errno, strerror(errno));
            return;
        }

        printf("\n============== Shm Info ==============\n");
        printf(" shm_key    : %d, %#x\n",m_shm_key, m_shm_key);
        printf(" shm_id     : %d, %#x\n",m_shm_id, m_shm_id);
        printf(" shm_addr   : %p\n",m_shm_addr);
        printf(" max_size   : %lu\n",m_size);
        printf(" total byte : %lu\n",shm_data.shm_segsz);
        printf(" attach_nums: %lu\n",shm_data.shm_nattch);
        printf("======================================\n");
    }
}
