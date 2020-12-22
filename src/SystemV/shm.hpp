#pragma one

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

namespace SystemV
{
    class Shm
    {
        public:
            Shm(const char* path, int proj_id, size_t size);
            ~Shm();
            bool Receive(char *buffer, int start_pos, size_t length);
            bool Send(const char *buffer, int start_pos, size_t length);
            void Clear(void);
            void Release(void);
            void Info(void);
        private:
            key_t m_shm_key;
            char *m_shm_addr;
            int   m_shm_id;
            size_t m_size;
    };
}

