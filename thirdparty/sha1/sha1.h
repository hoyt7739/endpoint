#ifndef SHA1_H
#define SHA1_H

#include <cstdint>
#include <string>
#include <iostream>

class SHA1 {
public:
    SHA1();
    SHA1(const std::string& s);
    SHA1(std::istream& is);

    void update(const std::string& s);
    void update(std::istream& is);
    std::string final();

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};

#endif
