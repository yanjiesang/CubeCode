#pragma once

#include <memory>

namespace Posix{
    // template<typename T>
    class Mmap{
        public:
            static std::unique_ptr<Mmap> Creat(const char *filename, int mmap_size, int mmap_prot, int mmap_flags);
            ~Mmap();

            char *GetMmapPtr(void);
            void Clean(void);
            ssize_t  GetMmapSize(void) const;
            void SetMmapSize(ssize_t size);
            int  GetMmapCapacity(void) const;
            int  Wirte(const char *dst, unsigned long start_pos, int write_size); 
            int  Read(char *dst, unsigned long start_pos, int read_size);      

        private:
            Mmap();
            bool Initialize(const char *filename, int mmap_size, int mmap_prot, int mmap_flags);
            
        private:
            int m_fd;
            int m_length;
            int m_prot;
            int m_flags;
            char *m_mmap_ptr;
    };
}
