#include <iostream>
#include <vector>

#include "bitmap.h"

int main(int argc, char * argv[]) {
    Bitmap<int> a(10, 10), b;
    b = a;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            a[i][j] = i * 10 + j;
    b = a;

    return 0;
}