#pragma once

#include <string>

#define FILE_CREAT_RW_ROOT_DIR  "./"

/* Common Interface */
void CompilerInfo(void);
std::string CombinationString(const char *format, ...);
std::string RunLinuxCmd(const char *cmd);

/* Directory Operator */
void ExtractFilePathAndName(const char *pInFileFullName, std::string &filepath,std::string &filename);
int  CreateDirectory(const char *pFileFullName);
void RemoveFile(const char *filename);

/* File Operator */
off_t   GetFileSize(const char *filename);
ssize_t WriteContentToFile(const char *filename,  int oflag, const char *content, size_t size);
ssize_t ReadContentFromFile(const char *filename, char **content);
std::string ReadContentFromFileRetString(const char *filename);
