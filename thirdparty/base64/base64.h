#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string base64_encode(const char* buffer, int size);
std::string base64_encode(const std::string& str);
std::string base64_decode(const std::string& encoded_str);

#endif
