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
        Bitmap<float> Y, YMean, YDiff, Cr, Cr2, Cb, Cb2;
        Process::toYCrCb(imIn.getRed(), imIn.getGreen(), imIn.getBlue(), Y, Cr, Cb);
        Process::filterMean(Y, YMean);
        Process::filterSub(Y, YDiff);
        Bitmap<unsigned char> YMeanQ, YDiffQ;
        Process::Quantify(YMean, YMeanQ, 7);
        Process::LogQuantify(YDiff, YDiffQ, 6);
        YMean = YMeanQ;
        YDiff = YDiffQ;
        Y.fill(YMean);
        Y.fill(YDiff, YDiff.width());
        Process::Reduce2(Cr, Cr2);
        Process::Reduce2(Cb, Cb2);
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
        Bitmap<float> Y, YMean, YDiff, Cr, Cr2, Cb, Cb2;
        Y = imIn.getRed();
        Y.copy(YMean, Y.width() / 2, Y.height());
        Y.copy(YDiff, Y.width() / 2, Y.height(), Y.width() / 2);
        Bitmap<unsigned char> YMeanQ, YDiffQ;
        YMeanQ = YMean;
        YDiffQ = YDiff;
        Process::Unquantify(YMeanQ, YMean, 7);
        Process::LogUnquantify(YDiffQ, YDiff, 6);
        Process::invertFilter(YMean, YDiff, Y);
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