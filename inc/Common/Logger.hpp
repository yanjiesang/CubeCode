#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <iostream>
#include <string>
#include "Common.hpp"

#define AUDIO_LOGGER_FILE           FILE_CREAT_RW_ROOT_DIR"log/audio.log"
#define VIDEO_LOGGER_FILE           FILE_CREAT_RW_ROOT_DIR"log/video.log"
#define LOGGER_FILE_NAME            FILE_CREAT_RW_ROOT_DIR"log/ygclient.log"

#define LOGGER_FILE_MAX_DEAL_NONE   0
#define LOGGER_FILE_MAX_DEAL_EMPTY  1
#define LOGGER_FILE_MAX_DEAL_WAY    LOGGER_FILE_MAX_DEAL_EMPTY

#if (LOGGER_FILE_MAX_DEAL_WAY!=LOGGER_FILE_MAX_DEAL_NONE)
    #define LOGGER_FILE_MAX_SIZE    1*1024*1024 //1M
#endif

std::string CreatLogTimestamp(void);
int LoggerFileCreat(const char *pFileFullName);
int LoggerFilePrintContent(std::string &LogContent, bool AddNewLine);
int LoggerFileWriteContent(const char *pFileFullName, std::string &LogContent, bool AddNewLine);

#ifdef SOFTWARE_LOGOUT_SWITCH
#define LOGOUT(type,fmt,args...)\
    do{\
        std::string LogoutStr;\
        LogoutStr=CombinationString("[%s]%s():%04d",#type,__func__,__LINE__);\
        LogoutStr+=" ";\
        LogoutStr+=CombinationString(fmt,##args);\
        LoggerFileWriteContent(LOGGER_FILE_NAME,LogoutStr, true);\
    }while(0);
#define LOG_OUT LOGOUT
#else
#define LOGOUT(type,fmt,args...)\
    do{\
        std::string LogoutStr;\
        LogoutStr=CombinationString("[%s]%s():%04d",#type,__func__,__LINE__);\
        LogoutStr+=" ";\
        LogoutStr+=CombinationString(fmt,##args);\
        LoggerFilePrintContent(LogoutStr, true);\
    }while(0);
#define LOG_OUT LOGOUT
#endif
#endif


