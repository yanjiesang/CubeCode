#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.hpp"

namespace SystemV
{
    Msg::Msg(const char *path, int proj_id)
    {
        m_msg_key=ftok(path,proj_id);
        if(m_msg_key == -1)
        {
            printf("Error: ftok\n");
        }

        m_msg_id=msgget(m_msg_key,IPC_CREAT|IPC_EXCL|0666);
        if(m_msg_id==-1)
        {
            if(errno==EEXIST)
            {
                m_msg_id=msgget(m_msg_key,IPC_CREAT|0666);
                if(m_msg_id==-1)
                {
                    printf("Error: msgget, errno:%d, errno info:%s\n",errno, strerror(errno));
                }
            }
            else
            {
                printf("Error: msgget, errno:%d, errno info:%s\n",errno, strerror(errno));
            }
        }

        m_msg_buff=NULL;
    }

    Msg::~Msg()
    {
        // 注释掉这里避免进程报错
        // struct msqid_ds tmsqid_ds;
        // if(msgctl(m_msg_id,IPC_RMID,&tmsqid_ds)==-1)
        // {
        //     printf("Error: msgctl, errno:%d, errno info:%s\n",errno, strerror(errno));
        // }

        if(m_msg_buff)
        {
            free(m_msg_buff);
            m_msg_buff=NULL;
        }
    }

    int Msg::Receive(long msgtype, char **buffer, size_t *length, bool block)
    {
        m_msg_buff = (Msg::MsgBuf *)calloc(1,sizeof(Msg::MsgBuf)+MESSGE_QUEUE_MAX_SZIE);
        if(m_msg_buff == NULL)
        {
            return -1;
        }

        if(msgrcv(m_msg_id,(void *)m_msg_buff,MESSGE_QUEUE_MAX_SZIE, msgtype, block?0:IPC_NOWAIT)==-1)
        {
            printf("Msg Receive Error\n");
            free(m_msg_buff);
            m_msg_buff=NULL;
            return -1;
        }

        *length=strlen(&m_msg_buff->mtext[0]);
        *buffer=(char *)malloc(*length+1);
        if(*buffer)
        {
            memset(*buffer,0,*length+1);
            memcpy(*buffer,&m_msg_buff->mtext[0],*length);
        }
        m_msg_type=m_msg_buff->mtype;

        if(m_msg_buff)
        {
            free(m_msg_buff);
            m_msg_buff=NULL;
        }

        return 0;
    }

    int Msg::Receive(long msgtype, std::string &buffer, bool block)
    {
        char *data=NULL;
        size_t length=0;

        if(Receive(msgtype, &data, &length, block)==-1)
        {
            return -1;
        }

        if(data)
        {
            buffer=data;
            free(data);
            data=NULL;
        }
        return 0;
    }

    int Msg::Send(long msgtype, const char *buffer, size_t length, bool block)
    {
        int ret_code=0;
        m_msg_buff = (Msg::MsgBuf *)calloc(1,sizeof(Msg::MsgBuf)+length);
        if(m_msg_buff == NULL)
        {
            return -1;
        }
        memset(m_msg_buff,0,length);

        m_msg_buff->mtype=msgtype;
        memcpy(m_msg_buff->mtext, buffer,length);
        m_msg_type=m_msg_buff->mtype;

        ret_code = msgsnd(m_msg_id,m_msg_buff,length,block?0:IPC_NOWAIT);
        if(ret_code == -1)
        {
            printf("Msg Send Error\n");
            free(m_msg_buff);
            m_msg_buff=NULL;
            return -1;
        }

        if(m_msg_buff)
        {
            free(m_msg_buff);
            m_msg_buff=NULL;
        }
        return 0;
    }

    int Msg::Send(long msgtype, std::string &buffer, bool block)
    {
        const char *data=buffer.c_str();
        size_t length=buffer.size();

        if(Send(msgtype, data, length,block)==-1)
        {
            return -1;
        }

        return 0;
    }

    void Msg::Delete(void)
    {
        struct msqid_ds tmsqid_ds;
        if(msgctl(m_msg_id,IPC_RMID,&tmsqid_ds)==-1)
        {
            printf("Error: msgctl, errno:%d, errno info:%s\n",errno, strerror(errno));
        }
    }

    long Msg::MsgType(void) const
    {
        return m_msg_type;
    }
}
