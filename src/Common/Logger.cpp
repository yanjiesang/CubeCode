#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "Logger.hpp"
#include "SoftwareConfig.h"

int LoggerFileWriteHeader(const char *pFileFullName)
{
    int write_num=0;
    int logger_file_fd;
    std::string HeaderContent;

    logger_file_fd=open(pFileFullName,O_WRONLY|O_APPEND);
    if(logger_file_fd==-1)
    {
        printf("[%s] %s():%d Logger file [%s] Open Failed, ErrorInfo:%s\n",__FILE__,__func__,__LINE__,pFileFullName,strerror(errno));
        return -1;
    }

    HeaderContent=CombinationString(
        "======================================================================\n"
        "    BuildTime      : %s %s                                            \n"
        "    ReleaseVersion : %s                                               \n"
        "    DebugVersion   : %s                                               \n"
        "    AppPlatform    : %s                                               \n"
        "======================================================================\n\n"
        ,__DATE__, __TIME__,SOFTWARE_RELEASE_VERSION,SOFTWARE_DEBUG_VERSION,(SOFTWARE_PLATFORM==SOFTWARE_PLATFORM_X86)?"x86_64":"arm"
    );

    write_num=write(logger_file_fd,HeaderContent.c_str(),HeaderContent.size());

    close(logger_file_fd);
}

std::string CreatLogTimestamp(void)
{
    std::string time_stamp;
    struct tm *real_time;
    char time_str[40];

    struct timeval tv;
    gettimeofday(&tv,NULL);
    real_time=localtime(&tv.tv_sec);
    sprintf(time_str,"[%04d-%02d-%02d %02d:%02d:%02d.%06ld]",real_time->tm_year+1900,real_time->tm_mon+1,real_time->tm_mday,
                real_time->tm_hour,real_time->tm_min,real_time->tm_sec,tv.tv_usec);
    time_stamp=time_str;
    return time_stamp;
}

off_t LoggerFileGetSize(const char *pFileFullName)
{
    struct stat logger_file_stat;

    if(stat(pFileFullName,&logger_file_stat)!=0)
    {
        printf("%s[ERROR] %s():%d Logger file [%s] Size Get Failed, ErrorInfo: %s\n",CreatLogTimestamp().c_str(),__func__,__LINE__,pFileFullName,strerror(errno));
        return -1;
    }

    return logger_file_stat.st_size;
}

int LoggerFileCreat(const char *pFileFullName)
{
    int ret_code=0;
    std::string filepath;
    std::string filename;
    DIR *pDIR;
    int logger_file_fd;

    CreateDirectory(pFileFullName);

    logger_file_fd=open(pFileFullName,O_RDWR|O_CREAT,0664);
    if(logger_file_fd==-1)
    {
        printf("%s[ERROR] %s():%d Logger File [%s] Creat Falied, ErrorInfo: %s\n",CreatLogTimestamp().c_str(),__func__,__LINE__,pFileFullName,strerror(errno));
        return -1;
    }

#if (LOGGER_FILE_MAX_DEAL_WAY!=LOGGER_FILE_MAX_DEAL_NONE)
    if(LoggerFileGetSize(pFileFullName)>LOGGER_FILE_MAX_SIZE)
    {
        ret_code=ftruncate(logger_file_fd,0);
    }
#endif
    close(logger_file_fd);

    LoggerFileWriteHeader(pFileFullName);

    return 0;
}

int LoggerFileWriteContent(const char *pFileFullName, std::string &LogContent, bool AddNewLine)
{
    int ret_code=0, write_num=0;
    std::string WriteContent;
    int logger_file_fd=-1;

#if (LOGGER_FILE_MAX_DEAL_WAY!=LOGGER_FILE_MAX_DEAL_NONE)
    if(LoggerFileGetSize(pFileFullName)>LOGGER_FILE_MAX_SIZE)
    {
        ret_code=ftruncate(logger_file_fd,0);
    }
#endif

    logger_file_fd=open(pFileFullName,O_WRONLY|O_APPEND);
    if(logger_file_fd==-1)
    {
        printf("%s[ERROR] %s():%d Logger File [%s] Creat Falied, ErrorInfo: %s\n",CreatLogTimestamp().c_str(),__func__,__LINE__,pFileFullName,strerror(errno));
        return -1;
    }

    WriteContent=CreatLogTimestamp();
    WriteContent+=LogContent;
    if(AddNewLine==true)
    {
        WriteContent+="\n";
    }

    write_num=write(logger_file_fd,WriteContent.c_str(),WriteContent.size());

    close(logger_file_fd);

    printf("%s",WriteContent.c_str());
}

int LoggerFilePrintContent(std::string &LogContent, bool AddNewLine)
{
    std::string WriteContent;

    WriteContent=CreatLogTimestamp();
    WriteContent+=LogContent;
    if(AddNewLine==true)
    {
        WriteContent+="\n";
    }

    printf("%s",WriteContent.c_str());
}