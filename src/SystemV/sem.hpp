#pragma one

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

namespace SystemV
{
    class Sem
    {
        public:
            Sem(const char* path, int proj_id, int *val, int signum);
            ~Sem();
            void Increase(int semno);
            void Decrease(int semno);
            void Delete(int semno);
            void Info(void);
        private:
            key_t m_sem_key;
            int m_sem_id;
            int m_semnums;
            union semun 
            {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO */
            };
    };
}
