#pragma once

#include <string>
#include "JsonParser.hpp"

namespace Utils{
    class Base64{
        public:
            static std::string Encode(const std::string& data);
            static std::string Decode(const std::string& data);
            static int Encode(char *pInputData, uint32_t inputLength, uint8_t * pOutputData, uint32_t *pOutputLength);
            static int Decode(char *pInputData, uint32_t inputLength, uint8_t * pOutputData, uint32_t *pOutputLength);
    };

    class Zlib{
        public:
            static int Compress(char *source , int source_len , char **dest, int *dest_len);
            static int Uncompress(char *source, int source_len , char *dest , int *dest_len);
    };
}
