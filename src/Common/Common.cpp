#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include "Common.hpp"
#include "Logger.hpp"
#include "SoftwareConfig.h"

void CompilerInfo(void)
{
    printf("=============================================================================\n");
    printf("    AppName        : %s                                                      \n",PROJECT_NAME);
    printf("    BuildTime      : %s %s                                                   \n",__DATE__, __TIME__);
    printf("    ReleaseVersion : %s                                                      \n",SOFTWARE_RELEASE_VERSION);
    printf("    DebugVersion   : %s                                                      \n",SOFTWARE_DEBUG_VERSION);
    printf("    AppPlatform    : %s                                                      \n",(SOFTWARE_PLATFORM==SOFTWARE_PLATFORM_X86)?"x86_64":"arm");
    printf("=============================================================================\n");
}

std::string CombinationString(const char *format, ...)
{
    std::string ret_string;
    va_list args;
    va_start(args,format);
    va_list temps;
    va_copy(temps,args);
    int length=vsnprintf(NULL,0,format,temps);
    va_end(temps);

    char *string=(char *)calloc(length+1,sizeof(char));
    if(string)
    {
        vsnprintf(string,length+1,format,args);
        ret_string=string;
        free(string);
        string=NULL;
    }
    va_end(args);
    return ret_string;
}

void ExtractFilePathAndName(const char *pInFileFullName, std::string &filepath,std::string &filename)
{
    std::string filepathtemp[1024];
    int dircount=0;
    DIR *pDIR=NULL;
    struct stat fileStat;

    int input_filename_length=strlen(pInFileFullName);
    char *pInFileFullNameTemp=(char *)calloc(input_filename_length,sizeof(char));
    memcpy(pInFileFullNameTemp,pInFileFullName,input_filename_length);

    if(strchr(pInFileFullNameTemp,'/')!=NULL)
    {
        if(pInFileFullNameTemp[input_filename_length-1]=='/')
        {
            filepath=pInFileFullNameTemp;
            filename.clear();        
        }
        else
        {
            char *strdeli=NULL;
            if(strchr(pInFileFullNameTemp,'/')!=NULL)
            {
                strdeli=strtok(pInFileFullNameTemp,"/");
                while(strdeli!=NULL)
                {
                    filepathtemp[dircount]=strdeli;
                    strdeli=strtok(NULL,"/");
                    dircount++;
                }
                if(pInFileFullNameTemp[0]=='/')
                {
                    filepathtemp[dircount]+='/';
                }
                for(int i=0;i<dircount-1;i++)
                {                    
                    filepathtemp[dircount]+=filepathtemp[i];
                    filepathtemp[dircount]+="/";
                }
            }

            filepath=filepathtemp[dircount];
            filename=filepathtemp[dircount-1]; 
        }
    }
    else
    {
        stat(pInFileFullNameTemp,&fileStat);
        if(S_ISDIR(fileStat.st_mode))
        {
            filepath=pInFileFullNameTemp;
            filename.clear();    
        }
        if(S_ISREG(fileStat.st_mode))
        {
            filepath.clear();
            filename=pInFileFullNameTemp;    
        }
    }

    if(pInFileFullNameTemp)
    {
        free(pInFileFullNameTemp);
        pInFileFullNameTemp=NULL;
    }
}

int ExtarctSubDirectory(const char *pDirectoryPath, char **pSubDirectoryList)
{
    char *directorypath_temp=NULL;
    char *ptemp=NULL;
    char *subdirectory=(char *)calloc(1,256*sizeof(char));
    char *psubdirectory=subdirectory;
    size_t directorypath_length=0;
    int subdirectory_cnt=0;
    if(pDirectoryPath==NULL)
    {
        return 0;
    }

    directorypath_length=strlen(pDirectoryPath);
    if(pDirectoryPath[directorypath_length-1]=='/')
    {
        directorypath_temp=(char *)calloc(1,directorypath_length);
        if(directorypath_temp==NULL)
        {
            return 0;
        }
        ptemp=directorypath_temp;
        memcpy(directorypath_temp,pDirectoryPath,directorypath_length);
    }
    else
    {
        directorypath_temp=(char *)calloc(1,directorypath_length+1);
        if(directorypath_temp==NULL)
        {
            return 0;
        }
        ptemp=directorypath_temp;
        memcpy(directorypath_temp,pDirectoryPath,directorypath_length);
        strcat(directorypath_temp,"/");
    }

    if(directorypath_temp[0]=='.')
    {
        subdirectory=(char *)".";
        memcpy(pSubDirectoryList[subdirectory_cnt],subdirectory,strlen(subdirectory));
        subdirectory_cnt++;
        directorypath_temp=directorypath_temp+2;
    }
    else if((directorypath_temp[0]=='/'))
    {
        subdirectory=(char *)"/";
        memcpy(pSubDirectoryList[subdirectory_cnt],subdirectory,strlen(subdirectory)); 
        subdirectory_cnt++;
        directorypath_temp++;
    }

    subdirectory=strtok(directorypath_temp,"/");
    while(subdirectory!=NULL)
    {
        memcpy(pSubDirectoryList[subdirectory_cnt],subdirectory,strlen(subdirectory));
        subdirectory_cnt++;
        subdirectory=strtok(NULL,"/");
    }
    directorypath_temp=ptemp;
    subdirectory=psubdirectory;      

    if(subdirectory)
    {
        free(subdirectory);
        subdirectory=NULL;
    }

    if(directorypath_temp)
    {
        free(ptemp);
        ptemp=NULL;
    }
    return subdirectory_cnt;
}

int CreateDirectory(const char *pFileFullName)
{
    int ret_code=0;
    std::string filepath;
    std::string filename;
    DIR *pDIR;
    
    char *SubDirectoryList[10];
    std::string filefullpath;
    int i,SubDirectoryCnt=0;

    ExtractFilePathAndName(pFileFullName,filepath,filename);
    if(filepath.size()!=0)
    {
        for(i=0;i<10;i++)
        {
            SubDirectoryList[i]=(char *)calloc(1,256*sizeof(char));
        }

        SubDirectoryCnt=ExtarctSubDirectory(filepath.c_str(), SubDirectoryList);
        if(strcmp(SubDirectoryList[0],".")==0)
        {
            filefullpath += SubDirectoryList[0];
            filefullpath += "/";
        }
        else if(strcmp(SubDirectoryList[0],"/")==0)
        {
            filefullpath += SubDirectoryList[0];
        }

        for(i=1;i<SubDirectoryCnt;i++)
        {
            filefullpath+=SubDirectoryList[i];
            filefullpath+="/";
            pDIR=opendir(filefullpath.c_str());
            if(pDIR==NULL)
            {
                ret_code=mkdir(filefullpath.c_str(),0775);
                if(ret_code==-1)
                {
                    printf("%s[ERROR] %s():%d Directory [%s] Creat Failed, ErrorInfo: %s\n",CreatLogTimestamp().c_str(),__func__,__LINE__,filefullpath.c_str(),strerror(errno));
                    return ret_code;
                }
                pDIR=opendir(filefullpath.c_str());
            }
            if(pDIR!=NULL)
            {
                closedir(pDIR);
            }
        }

        for(i=0;i<10;i++)
        {
            if(SubDirectoryList[i])
            {
                free(SubDirectoryList[i]);
                SubDirectoryList[i]=NULL;
            }
        }
    }

    return 0;
}

void RemoveFile(const char *filename)
{
    if(filename==NULL)
    {
        return;
    }

    if(access(filename,F_OK)==0)
    {
        if(remove(filename)==-1)
        {
            LOG_OUT(ERROR,"Remove File %s Error, %s",filename,strerror(errno));
        }
    }
}

ssize_t WriteContentToFile(const char *filename, int oflag, const char *content, size_t size)
{
    ssize_t write_num=0;
    int fd = -1;
    if(filename == NULL || content==NULL)
    {
        return -1;
    }
    if(oflag & O_CREAT == O_CREAT)
    {
        if((fd = open(filename,oflag,0664))==-1)
        {
            return -1;
        }
    }
    else
    {
        if((fd = open(filename,oflag))==-1)
        {
            return -1;
        }
    }
   
    write_num = write(fd,content,size);
    sync();
    close(fd);
    return write_num;
}

off_t GetFileSize(const char *filename)
{
    struct stat filestat;

    if(stat(filename,&filestat)==-1)
    {
        return 0;
    }

    return filestat.st_size;
}

ssize_t ReadContentFromFile(const char *filename, char **content)
{
    int fd = -1;
    struct stat filestat;
    ssize_t read_size=0;
    if(filename==NULL)
    {
        return -1;
    }
    if((fd=open(filename,O_RDONLY))==-1)
    {
        return -1;
    }
    memset(&filestat,0,sizeof(struct stat));
    fstat(fd, &filestat);
    *content = (char *)calloc(1,filestat.st_size);
    read_size=read(fd,*content,filestat.st_size);
    if(read_size!=filestat.st_size)
    {
        return -1;
    }
    return read_size;
}

std::string ReadContentFromFileRetString(const char *filename)
{
    std::string ret_content;
    int fd = -1;
    struct stat filestat;
    char *content=NULL;
    ssize_t read_size=0;
    if(filename==NULL)
    {
        return ret_content;
    }
    if((fd=open(filename,O_RDONLY))==-1)
    {
        return ret_content;
    }
    memset(&filestat,0,sizeof(struct stat));
    fstat(fd, &filestat);
    content = (char *)calloc(1,filestat.st_size);
    read_size=read(fd,content,filestat.st_size);
    if(read_size!=filestat.st_size)
    {
        return ret_content;
    }

    ret_content = content;
    if(content!=NULL)
    {
        free(content);
        content=NULL;
    }
    return ret_content;
}

std::string RunLinuxCmd(const char *cmd)
{
    std::string ret_str;
    FILE *fp=popen(cmd,"r");
    if(fp)
    {
        char buffer[1024];
        memset(buffer,0,sizeof(buffer));
        while(fgets(buffer,sizeof(buffer),fp))
        {
            ret_str+=buffer;
            memset(buffer,0,sizeof(buffer));
        }
        pclose(fp);
    }    
    return ret_str;
}
