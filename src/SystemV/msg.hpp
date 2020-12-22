#pragma one

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>

#define MESSGE_QUEUE_MAX_SZIE 8*1024

namespace SystemV
{
    class Msg
    {
        public:
            typedef struct 
            {
                long mtype;       /* message type, must be > 0 */
                char mtext[0];    /* message data */
            }MsgBuf;

            Msg(const char *path, int proj_id);
            ~Msg();

            int Receive(long msgtype, char **buffer, size_t *length, bool block=true);
            int Receive(long msgtype, std::string &buffer, bool block=true);
            int Send(long msgtype, const char *buffer, size_t length, bool block=false);
            int Send(long msgtype, std::string &buffer, bool block=false);
            void Delete(void);
            long MsgType(void) const;

        private:
            MsgBuf *m_msg_buff;
            long m_msg_type;
            key_t m_msg_key;
            int m_msg_id;
    };
}
