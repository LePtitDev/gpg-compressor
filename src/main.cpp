#include <iostream>
#include <vector>

#include "bitmap.h"
#include "process.h"
#include "filter.h"

int main(int argc, char * argv[]) {
    Bitmap<unsigned char> a(10, 10), b;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            a[i][j] = i * 10 + j;
    Filter::up(a, b);

    return 0;
}