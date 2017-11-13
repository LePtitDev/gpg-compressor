#include <iostream>

#include "format/image-ppm.h"
#include "process.h"

int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cerr << "usage : " << argv[0] << " -[c|d|p] <input.[pgm|ppm]> <output.[pgm|ppm]>" << std::endl;
        std::cerr << "- c : compress" << std::endl;
        std::cerr << "- d : decompress" << std::endl;
        std::cerr << "- p : psnr calculate" << std::endl;
        return -1;
    }

    ImagePPM imIn;
    imIn.load(argv[2]);
    ImagePPM imOut(imIn.width(), imIn.height());
    if (argv[1][1] == 'c') {
        Bitmap<float> Y, YMean, YDif, Cr, Cr2, Cb, Cb2;
        Process::toYCrCb(imIn.getRed(), imIn.getGreen(), imIn.getBlue(), Y, Cr, Cb);
        Process::Reduce2(Cr, Cr2);
        Process::Reduce2(Cb, Cb2);
        //Process::filterMean(Y, YMean);
        //Process::filterSub(Y, YDif);
        Cr2.resize(Cr.width(), Cr.height());
        Cb2.resize(Cb.width(), Cb.height());
        Bitmap<unsigned char> R, G, B;
        R = Y;
        Process::Quantify(Cr2, G, 7);
        Process::Quantify(Cb2, B, 7);
        imOut.setRed(R);
        imOut.setGreen(G);
        imOut.setBlue(B);
    }
    else if (argv[1][1] == 'd') {
        Bitmap<float> Y, Cr, Cr2, Cb, Cb2;
        Y = imIn.getRed();
        Process::Unquantify(imIn.getGreen(), Cr2, 7);
        Process::Unquantify(imIn.getBlue(), Cb2, 7);
        Cr2.resize(Y.width() / 2, Y.height() / 2);
        Cb2.resize(Y.width() / 2, Y.height() / 2);
        Process::Enlarge2(Cr2, Cr);
        Process::Enlarge2(Cb2, Cb);
        Bitmap<unsigned char> R, G, B;
        Process::toRGB(Y, Cr, Cb, R, G, B);
        imOut.setRed(R);
        imOut.setGreen(G);
        imOut.setBlue(B);
    }
    else {
        imOut.load(argv[3]);
        std::cout << "PSNR = " << Process::calculatePSNR(imIn.getGrayscale(), imOut.getGrayscale()) << std::endl;
    }
    imOut.save(argv[3]);

    return 0;
}