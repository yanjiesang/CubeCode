
#pragma once

#include <cstdint>
#include <cstring>
#include "./rapidjson/document.h"

namespace Utils
{
    class JsonParser
    {
        public:
            using JsonDocument = rapidjson::Document;
            using JsonValue = rapidjson::Value;
            using String = std::basic_string<char, std::char_traits<char>>;
        public:
            typedef enum
            {
                // Generic Response Codes
                SUCCESS = 0,                                              ///< Success return value - no error occurred.
                FAILURE = -1,                                             ///< A generic error. Not enough information for a specific error code.
                NULL_VALUE_ERROR = -2,                                    ///< A required parameter was passed as null.

                // I/O Error Codes
                FILE_OPEN_ERROR = -100,                                    ///< Unable to open the requested file
                FILE_NAME_INVALID = -101,                                  ///< File name is invalid or of zero length

                // JSON Parsing Error Codes
                JSON_PARSE_KEY_NOT_FOUND_ERROR = -800,                     ///< JSON Parser was not able to find the requested key in the specified JSON
                JSON_PARSE_KEY_UNEXPECTED_TYPE_ERROR = -801,               ///< The value type was different from the expected type
                JSON_PARSING_ERROR = -802,                                 ///< An error occurred while parsing the JSON string.  Usually malformed JSON.
                JSON_MERGE_FAILED = -803,                                  ///< Returned when the JSON merge request fails unexpectedly
                JSON_DIFF_FAILED = -804,                                   ///< Returned when the JSON diff request fails unexpectedly
            }ResponseCode;
        public:
            static ResponseCode InitializeFromJsonFile(JsonDocument &json_document,
                                                    const String &input_file_path);

            static ResponseCode InitializeFromJsonString(JsonDocument &json_document,
                                                        const String &input_json_string);

            static ResponseCode GetBoolValue(const JsonDocument &json_document, const char *key, bool &value);

            static ResponseCode GetIntValue(const JsonDocument &json_document, const char *key, int &value);

            static ResponseCode GetUint16Value(const JsonDocument &json_document, const char *key,
                                            uint16_t &value);

            static ResponseCode GetUint32Value(const JsonDocument &json_document, const char *key,
                                            uint32_t &value);

            static ResponseCode GetSizeTValue(const JsonDocument &json_document, const char *key, size_t &value);

            static ResponseCode GetCStringValue(const JsonDocument &json_document, const char *key, char *value,
                                                uint16_t max_string_len);

            static ResponseCode GetStringValue(const JsonDocument &json_document, const char *key,
                                            String &value);

            static rapidjson::ParseErrorCode GetParseErrorCode(const JsonDocument &json_document);

            static size_t GetParseErrorOffset(const JsonDocument &json_document);

            static ResponseCode MergeValues(JsonValue &target, JsonValue &source, JsonValue::AllocatorType &allocator);

            static ResponseCode DiffValues(JsonValue &target_doc,
                                        JsonValue &old_doc,
                                        JsonValue &new_doc,
                                        JsonValue::AllocatorType &allocator);

            static String ToString(JsonDocument &json_document);

            static String ToString(JsonValue &json_value);

            static ResponseCode WriteToFile(JsonDocument &json_document, const String &output_file_path);
    };
}

