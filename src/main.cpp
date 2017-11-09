#include <iostream>

#include "format/image-ppm.h"
#include "process.h"

int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cerr << "usage : " << argv[0] << "-[c|d] <input.[pgm|ppm]> <output.[pgm|ppm]>" << std::endl;
        std::cerr << "- c : compress" << std::endl;
        std::cerr << "- d : decompress" << std::endl;
        return -1;
    }

    ImagePPM imIn;
    imIn.load(argv[2]);
    ImagePPM imOut(imIn.width(), imIn.height());
    if (argv[1][1] == 'c') {
        Bitmap<float> Y, Cr, Cb;
        Process::toYCrCb(imIn.getRed(), imIn.getGreen(), imIn.getBlue(), Y, Cr, Cb);
        Bitmap<unsigned char> R, G, B;
        R = Y;
        G = Cr;
        B = Cb;
        imOut.setRed(R);
        imOut.setGreen(G);
        imOut.setBlue(B);
    }
    else {
        Bitmap<float> Y, Cr, Cb;
        Y = imIn.getRed();
        Cr = imIn.getGreen();
        Cb = imIn.getBlue();
        Bitmap<unsigned char> R, G, B;
        Process::toRGB(Y, Cr, Cb, R, G, B);
        imOut.setRed(R);
        imOut.setGreen(G);
        imOut.setBlue(B);
    }
    imOut.save(argv[3]);

    return 0;
}