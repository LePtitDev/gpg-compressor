#include <iostream>
#include <vector>

#include "process.h"
#include "format/image-ppm.h"

int main(int argc, char * argv[]) {
    ImagePPM img;
    img.load("res/coati.ppm");
    ImagePPM out(0, 0, false);
    Bitmap<unsigned char> red = img.getColor(0);
    out.resize(red.width(), red.height());
    for (unsigned int i = 0, h = red.width(); i < h; i++)
        for (unsigned int j = 0, w = red.width(); j < w; j++)
            out[i][j].r = red[i][j];
    out.save("result.pgm");

    return 0;
}