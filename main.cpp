#include <iostream>
#include <vector>

#include "bitmap.h"

int main(int argc, char * argv[]) {
    Bitmap<std::vector<int>> b(10, 10);
    b[0]->size();
    const Bitmap<std::vector<int>> c(b);
    c[0][0];

    return 0;
}