#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.hpp"
#include "./zlib-1.2.11/zlib.h"

namespace Utils{

    static const std::string B64Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    static bool is_base64(unsigned char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string Base64::Encode(const std::string& data)
    {
        std::string result;
        int i = 0;
        unsigned char ch_arr_3[3];
        unsigned char ch_arr_4[4];

        size_t encode_len = 0;
        size_t data_len = data.size();
        while (data_len--)
        {
            ch_arr_3[i++] = data[encode_len++];
            if (i == 3)
            {
                ch_arr_4[0] = (ch_arr_3[0] & 0xfc) >> 2;
                ch_arr_4[1] = ((ch_arr_3[0] & 0x3) << 4) + ((ch_arr_3[1] & 0xf0) >> 4);
                ch_arr_4[2] = ((ch_arr_3[1] & 0xf) << 2) + ((ch_arr_3[2] & 0xc0) >> 6);
                ch_arr_4[3] = ch_arr_3[2] & 0x3f;

                for (i = 0; i < 4; ++i)
                {
                    result += B64Alpha[ch_arr_4[i]];
                }
                i = 0;
            }
        }

        if (i > 0)
        {
            for (int j = i; j < 3; ++j)
            {
                ch_arr_3[j] = '\0';
            }

            ch_arr_4[0] = (ch_arr_3[0] & 0xfc) >> 2;
            ch_arr_4[1] = ((ch_arr_3[0] & 0x3) << 4) + ((ch_arr_3[1] & 0xf0) >> 4);
            ch_arr_4[2] = ((ch_arr_3[1] & 0xf) << 2) + ((ch_arr_3[2] & 0xc0) >> 6);

            for (int j = 0; j < i + 1; ++j)
            {
                result += B64Alpha[ch_arr_4[j]];
            }

            while (i++ < 3)
            {
                result += '=';
            }
        }
        return result;
    }

    std::string Base64::Decode(const std::string& data)
    {
        std::string result;
        int i = 0;
        unsigned char ch_arr_3[3];
        unsigned char ch_arr_4[4];

        size_t decode_len = 0;
        size_t data_len = data.size();
        while( data_len-- && (data[decode_len] != '=') && is_base64(data[decode_len]) )
        {
            ch_arr_4[i++] = data[decode_len++];
            if (i == 4)
            {
                for (i = 0; i < 4; ++i)
                {
                    ch_arr_4[i] = B64Alpha.find(ch_arr_4[i]);
                }

                ch_arr_3[0] = (ch_arr_4[0] << 2) + ((ch_arr_4[1] & 0x30) >> 4);
                ch_arr_3[1] = ((ch_arr_4[1] & 0xf) << 4) + ((ch_arr_4[2] & 0x3c) >> 2);
                ch_arr_3[2] = ((ch_arr_4[2] & 0x3) << 6) + ch_arr_4[3];

                for (i = 0; i < 3; ++i)
                {
                    result += ch_arr_3[i];
                }
                i = 0;
            }
        }

        if (i > 0)
        {
            for (int j = 0; j < i; ++j)
            {
                ch_arr_4[j] = B64Alpha.find(ch_arr_4[j]);
            }

            ch_arr_3[0] = (ch_arr_4[0] << 2) + ((ch_arr_4[1] & 0x30) >> 4);
            ch_arr_3[1] = ((ch_arr_4[1] & 0xf) << 4) + ((ch_arr_4[2] & 0x3c) >> 2);

            for (int j = 0; j < i - 1; ++j)
            {
                result += ch_arr_3[j];
            }
        }

        return result;
    }

    char BASE64_ENCODE_ALPHA[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    uint8_t BASE64_DECODE_ALPHA[256] = {
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //10
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //20
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //30
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //40
        0,   0,   0,  62,   0,   0,   0,  63,  52,  53, //50
        54,  55,  56,  57,  58,  59,  60,  61,   0,   0, //60
        0,   0,   0,   0,   0,   0,   1,   2,   3,   4, //70
        5,   6,   7,   8,   9,  10,  11,  12,  13,  14, //80
        15,  16,  17,  18,  19,  20,  21,  22,  23,  24, //90
        25,   0,   0,   0,   0,   0,   0,  26,  27,  28, //100
        29,  30,  31,  32,  33,  34,  35,  36,  37,  38, //110
        39,  40,  41,  42,  43,  44,  45,  46,  47,  48, //120
        49,  50,  51,   0,   0,   0,   0,   0,   0,   0, //130
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //140
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //150
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //160
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //170
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //180
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //190
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //200
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //210
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //220
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //230
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //240
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //250
        0,   0,   0,   0,   0,   0,
    };

    int Base64::Encode(char *pInputData, uint32_t inputLength, uint8_t * pOutputData, uint32_t *pOutputLength)
    {
        uint8_t BASE64_ENCODE_PADDING[3] = {0, 2, 1};

        uint32_t mod3;
        uint32_t outputLength;
        uint32_t padding;
        uint32_t i;
        uint8_t b0, b1, b2;
        uint8_t *pInput = (uint8_t *) pInputData;
        uint8_t * pOutput = pOutputData;

        if (pInputData == NULL || pOutputLength == NULL) {
            return -1;
        }

        if (inputLength == 0) {
            return -1;
        }

        // Calculate the needed buffer size
        mod3 = inputLength % 3;
        padding = BASE64_ENCODE_PADDING[mod3];

        // Include the NULL terminator in the length calculation
        outputLength = 4 * (inputLength + padding) / 3;

        // Need to have at least a triade to process in the loop
        if (inputLength >= 3) {
            for (i = 0; i <= inputLength - 3; i += 3) {
                b0 = *pInput++;
                b1 = *pInput++;
                b2 = *pInput++;

                *pOutput++ = BASE64_ENCODE_ALPHA[b0 >> 2];
                *pOutput++ = BASE64_ENCODE_ALPHA[((0x03 & b0) << 4) + (b1 >> 4)];
                *pOutput++ = BASE64_ENCODE_ALPHA[((0x0f & b1) << 2) + (b2 >> 6)];
                *pOutput++ = BASE64_ENCODE_ALPHA[0x3f & b2];
            }
        }

        if (padding == 1) {
            *pOutput++ = BASE64_ENCODE_ALPHA[*pInput >> 2];
            *pOutput++ = BASE64_ENCODE_ALPHA[((0x03 & *pInput) << 4) + (*(pInput + 1) >> 4)];
            *pOutput++ = BASE64_ENCODE_ALPHA[(0x0f & *(pInput + 1)) << 2];
            *pOutput++ = '=';
        } else if (padding == 2) {
            *pOutput++ = BASE64_ENCODE_ALPHA[*pInput >> 2];
            *pOutput++ = BASE64_ENCODE_ALPHA[(0x03 & *pInput) << 4];
            *pOutput++ = '=';
            *pOutput++ = '=';
        }

        *pOutputLength = outputLength;

        return 0;
    }

    int Base64::Decode(char * pInputData, uint32_t inputLength, uint8_t * pOutputData, uint32_t *pOutputLength)
    {
        uint8_t BASE64_DECODE_PADDING[4] = {0, 0xff, 2, 1};
        uint32_t outputLength;
        uint32_t i;
        uint8_t b0, b1, b2, b3;
        uint32_t padding = 0;
        uint8_t *pInput = (uint8_t *) pInputData;
        uint8_t *pOutput = pOutputData;

        if (pInput == NULL || pOutputLength == NULL) {
            return -1;
        }

        inputLength = (inputLength != 0) ? inputLength : (uint32_t) strlen(pInputData);
        // Check the size - should have more than 2 chars
        if (inputLength < 2) {
            return -1;
        }

        // Check the padding
        if (pInput[inputLength - 1] == '=') {
            inputLength--;
        }

        // Check for the second padding
        if (pInput[inputLength - 1] == '=') {
            inputLength--;
        }

        // Calculate the padding
        padding = BASE64_DECODE_PADDING[inputLength % 4];

        // Mod4 can't be 1 which means the padding can never be 0xff
        if (padding == 0xff) {
            return -1;
        }

        // Calculate the output length
        outputLength = 3 * inputLength / 4;

        // Proceed with the decoding - we should have at least a quad to process in the loop
        if (inputLength >= 4) {
            for (i = 0; i <= inputLength - 4; i += 4) {
                b0 = BASE64_DECODE_ALPHA[*pInput++];
                b1 = BASE64_DECODE_ALPHA[*pInput++];
                b2 = BASE64_DECODE_ALPHA[*pInput++];
                b3 = BASE64_DECODE_ALPHA[*pInput++];

                *pOutput++ = (b0 << 2) | (b1 >> 4);
                *pOutput++ = (b1 << 4) | (b2 >> 2);
                *pOutput++ = (b2 << 6) | b3;
            }
        }

        // Process the padding
        if (padding == 1) {
            b0 = BASE64_DECODE_ALPHA[*pInput++];
            b1 = BASE64_DECODE_ALPHA[*pInput++];
            b2 = BASE64_DECODE_ALPHA[*pInput++];

            *pOutput++ = (b0 << 2) | (b1 >> 4);
            *pOutput++ = (b1 << 4) | (b2 >> 2);
        } else if (padding == 2) {
            b0 = BASE64_DECODE_ALPHA[*pInput++];
            b1 = BASE64_DECODE_ALPHA[*pInput++];

            *pOutput++ = (b0 << 2) | (b1 >> 4);
        }

        // Set the correct size
        *pOutputLength = outputLength;

        return 0;
    }

int Zlib::Compress(char *source , int source_len , char **dest, int *dest_len)
{
    int ret = -1;
    if (!source_len)
        return ret;

    int max_len = source_len * 2;
    *dest = (char*)malloc(max_len);
    if (!*dest)
    {
        return ret;
    }
    memset(*dest,0,max_len);

    //先对原始内容进行压缩工作
    z_stream c_stream;
    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    c_stream.next_in = (Bytef*)source;
    c_stream.avail_in = source_len;
    c_stream.next_out = (Bytef*)*dest;
    c_stream.avail_out = (unsigned int)max_len;

    ret = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK)
    {
        printf("init error");
        return ret;
    }

    ret = deflate(&c_stream , Z_FINISH);
    if(ret != Z_STREAM_END)
    {
        deflateEnd(&c_stream);
        printf("stream  error");
        return ret;
    }
    *dest_len = c_stream.total_out;
    ret = deflateEnd(&c_stream);
    if(ret != Z_OK)
    {
        printf("End  error\n");
        return ret;
    }

    return ret;
}

int Zlib::Uncompress(char *source, int source_len , char *dest , int *dest_len)
{
    int ret;
    unsigned have;
    z_stream strm;
    int max_len = 200 * 1024;
    unsigned char out[max_len];
    int totalsize = 0;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit(&strm);
    if (ret != Z_OK)
    {
        printf("init error\n");
        return ret;
    }

    strm.avail_in = source_len;
    strm.next_in = (unsigned char*)source;

    do
    {
        strm.avail_out = max_len;
        strm.next_out = out;
        ret = inflate(&strm,Z_NO_FLUSH);
        switch(ret)
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
                printf("data error\n");
                inflateEnd(&strm);
                return ret;
            case Z_MEM_ERROR:
                printf("memory error\n");
                inflateEnd(&strm);
                return ret;
        }
        have = max_len - strm.avail_out;
        totalsize += have;
        memcpy(dest+totalsize-have,out,totalsize);
    }while(strm.avail_out == 0);

    inflateEnd(&strm);
    *dest_len = totalsize;
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

}