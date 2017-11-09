#include <iostream>
#include <vector>

#include "huffman.h"
#include "process.h"
#include "format/image-ppm.h"

int main(int argc, char * argv[]) {
    Huffman<> huff;
    std::bitset<8> bs(0x12);
    for (int i = 0; i < 8; i++)
        std::cout << (int)bs[i];
    std::cout << std::endl;

    return 0;
}