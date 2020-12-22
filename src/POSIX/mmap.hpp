#pragma once

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <memory>

namespace Posix{
    template<typename T>
    class Mmap{
        public:
            static std::unique_ptr<Mmap<T>> Creat(const char *filename, int mmap_size, int mmap_prot, int mmap_flags);
            ~Mmap();

            T *GetMmapPtr(void);
            void Clean(void);
            ssize_t  GetMmapSize(void) const;
            void SetMmapSize(ssize_t size);
            int  GetMmapCapacity(void) const;
            int  Wirte(const T *dst, unsigned long start_pos, int write_size); 
            int  Read(T *dst, unsigned long start_pos, int read_size);      

        private:
            Mmap();
            bool Initialize(const char *filename, int mmap_size, int mmap_prot, int mmap_flags);
            
        private:
            int m_fd;
            int m_length;
            int m_prot;
            int m_flags;
            T *m_mmap_ptr;
    };

    template<typename T>
    std::unique_ptr<Mmap<T>> Mmap<T>::Creat(const char* filename, int mmap_size, int mmap_prot, int mmap_flags)
    {
        std::unique_ptr<Mmap<T>> cMmap(new Mmap<T>());
        if(!cMmap->Initialize(filename, mmap_size, mmap_prot, mmap_flags))
        {
            return nullptr;
        }
        return cMmap;
    }

    template<typename T>
    T *Mmap<T>::GetMmapPtr(void)
    {
        return m_mmap_ptr;
    }

    template<typename T>
    void Mmap<T>::Clean(void)
    {
        memset(m_mmap_ptr,0,m_length);
    }

    template<typename T>
    ssize_t Mmap<T>::GetMmapSize(void) const
    {
        ssize_t size=0;
        for(int i=0;i<sizeof(ssize_t);i++)
        {
            size+=(m_mmap_ptr[i]&0xFF)<<(8*i);
        }
        return size;
    }

    template<typename T>
    void Mmap<T>::SetMmapSize(ssize_t size)
    {
        for(int i=0;i<sizeof(ssize_t);i++)
        {
            m_mmap_ptr[i]=(size>>(8*i))&0xFF;
        }
    }

    template<typename T>
    int Mmap<T>::GetMmapCapacity(void) const
    {
        return m_length;
    }

    template<typename T>
    int Mmap<T>::Wirte(const T *dst, unsigned long start_pos, int write_size)
    {
        memcpy(m_mmap_ptr+start_pos,dst,write_size);
    }

    template<typename T>
    int Mmap<T>::Read(T *dst, unsigned long start_pos, int read_size)
    {
        memcpy(dst,m_mmap_ptr+start_pos,read_size);
        return m_length-start_pos;
    }

    template<typename T>
    bool Mmap<T>::Initialize(const char* filename, int mmap_size, int mmap_prot, int mmap_flags)
    {
        int ret;
        m_fd=-1;
        if(access(filename,F_OK)!=0)
        {
            m_fd=open(filename,O_RDWR|O_CREAT,0664);
            if(m_fd==-1)
            {
                printf("Error: Mmap open %s, errno info:%s\n",filename, strerror(errno));
                return false;
            }
        }
        else
        {
            m_fd=open(filename,O_RDWR);
            if(m_fd==-1)
            {
                printf("Error: Mmap open %s, errno info:%s\n",filename, strerror(errno));
                return false;
            }
        }
        ret = ftruncate(m_fd,mmap_size);
        if(m_fd!=-1)
        {
            m_mmap_ptr=(T *)mmap(NULL,mmap_size,mmap_prot,mmap_flags,m_fd,0);
            if(m_mmap_ptr==MAP_FAILED)
            {
                printf("Error: creat mmap error, errno info:%s\n",strerror(errno));
                close(m_fd);
                return false;
            }
            close(m_fd);
        }
        return true;
    }

    template<typename T>
    Mmap<T>::Mmap()
    {
        m_fd=-1;
        m_length=0;
        m_prot=0;
        m_flags=0;
        m_mmap_ptr=nullptr;
    }

    template<typename T>
    Mmap<T>::~Mmap()
    {
        if(m_fd!=-1)
        {
            close(m_fd);
        }

        if(m_mmap_ptr!=NULL)
        {
            munmap(m_mmap_ptr,m_length);
        }
    }

}
