#include <iostream>
#include <fstream>

#include "format/image-ppm.h"
#include "process.h"
#include "stream.h"

using namespace LiteScript;

void compress(const char * infile, const char * outfile);
void decompress(const char * infile, const char * outfile);

int main(int argc, char * argv[]) {
    if (argc < 4) {
        std::cerr << "usage : " << argv[0] << " -[c|d|p] <input.[pgm|ppm]> <output.[pgm|ppm]>" << std::endl;
        std::cerr << "- c : compress" << std::endl;
        std::cerr << "- d : decompress" << std::endl;
        return -1;
    }

    if (argv[1][1] == 'c')
        compress(argv[2], argv[3]);
    else if (argv[1][1] == 'd')
        decompress(argv[2], argv[3]);
    else {
        ImagePPM imIn;
        imIn.load(argv[2]);
        ImagePPM imOut(imIn.width(), imIn.height());
        imOut.load(argv[3]);
        Bitmap<unsigned char> Y1, Y2;
        if (imIn.colored()) {
            Bitmap<unsigned char> R1, G1, B1, R2, G2, B2;
            R1 = imIn.getRed();
            G1 = imIn.getGreen();
            B1 = imIn.getBlue();
            R2 = imOut.getRed();
            G2 = imOut.getGreen();
            B2 = imOut.getBlue();
            Process::toGrayscale(R1, G1, B1, Y1);
            Process::toGrayscale(R2, G2, B2, Y2);
        }
        else {
            Y1 = imIn.getGrayscale();
            Y2 = imOut.getGrayscale();
        }
        std::cout << "PSNR = " << Process::calculatePSNR(Y1, Y2) << std::endl;
    }

    return 0;
}

void compressColor(OStreamer& stream, const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B);
void compressGrayscale(OStreamer& stream, const Bitmap<unsigned char>& map);

void compress(const char * infile, const char * outfile) {
    ImagePPM imIn;
    if (!imIn.load(infile)) {
        std::cerr << "erreur : Impossible de lire l'image" << std::endl;
        exit(0);
    }

    std::ofstream file(outfile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "erreur : Impossible d'écrire sur l'image compresse" << std::endl;
        exit(0);
    }
    OStreamer stream(file);

    stream << imIn.width() << imIn.height();
    if (imIn.colored()) {
        stream << (char)1;
        compressColor(stream, imIn.getRed(), imIn.getGreen(), imIn.getBlue());
    }
    else {
        stream << (char)0;
        compressGrayscale(stream, imIn.getGrayscale());
    }

    file.close();
}

void saveBitvector(OStreamer& stream, std::vector<bool>& bitvector);

void compressColor(OStreamer& stream, const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B) {
    Bitmap<unsigned char> YQ, YMeanQ, YDiffQ, YDiffQ2, CrQ, CbQ, R2, G2, B2;
    Bitmap<float> Y, Y2, YMean, YDiff, Cr, Cr2, Cb, Cb2;
    std::vector<bool> bitvector;

    Process::toYCrCb(R, G, B, Y, Cr, Cb);
    Process::filterMean(Y, YMean);
    Process::filterSub(Y, YDiff);
    Process::Quantify(YMean, YMeanQ, 7);
    Process::LogQuantify(YDiff, YDiffQ, 6);
    Process::ReduceQuantify(YDiffQ, YDiffQ, 2);
    Process::Reduce2(Cr, Cr2);
    Process::Reduce2(Cb, Cb2);
    Process::Quantify(Cr2, CrQ, 7);
    Process::Quantify(Cb2, CbQ, 7);
    
    Process::Unquantify(CrQ, Cr2, 7);
    Process::Unquantify(CbQ, Cb2, 7);
    Process::Enlarge2(Cr2, Cr);
    Process::Enlarge2(Cb2, Cb);
    Process::EnlargeQuantify(YDiffQ, YDiffQ2, 2);
    Process::LogUnquantify(YDiffQ2, YDiff, 6);
    Process::Unquantify(YMeanQ, YMean, 7);
    Process::invertFilter(YMean, YDiff, Y2);
    Process::toRGB(Y2, Cr, Cb, R2, G2, B2);
    Process::toGrayscale(R2, G2, B2, YDiffQ2);
    YQ = Y;

    Process::grayCoding(CrQ, CrQ);
    Process::grayCoding(CbQ, CbQ);
    if (Process::calculatePSNR(YQ, YDiffQ2) >= 35.0f) {
        stream << (unsigned char)1;
        Process::grayCoding(YMeanQ, YMeanQ);
        Process::huffman(YMeanQ, bitvector, 3);
        Process::arithmeticEncoding(YMeanQ, bitvector, 7, 3, 16);
        Process::huffman(YDiffQ, bitvector, 4);
    }
    else {
        stream << (unsigned char)2;
        Process::mergeGrayscale(YQ, YMeanQ, 64);
        Y = YMeanQ;
        Process::Quantify(Y, YMeanQ, 6);
        Process::huffman(YMeanQ, bitvector, 3);
        Process::arithmeticEncoding(YMeanQ, bitvector, 6, 3, 16);
    }

    Process::huffman(CrQ, bitvector, 2);
    Process::arithmeticEncoding(CrQ, bitvector, 7, 2, 16);
    Process::huffman(CbQ, bitvector, 2);
    Process::arithmeticEncoding(CbQ, bitvector, 7, 2, 16);
    saveBitvector(stream, bitvector);
}

void compressGrayscale(OStreamer& stream, const Bitmap<unsigned char>& map) {
    Bitmap<float> Y;
    Bitmap<unsigned char> YQ;
    Process::mergeGrayscale(map, YQ, 64);
    Y = YQ;
    Process::Quantify(Y, YQ, 6);
    std::vector<bool> bitvector1, bitvector2;
    unsigned int C1, C2;
    C1 = Process::huffman(YQ, bitvector1, 3);
    C1 += Process::arithmeticEncoding(YQ, bitvector1, 6, 3, 16);
    C2 = Process::huffman(YQ, bitvector2, 6);
    if (C1 < C2) {
        stream << (unsigned char)1;
        saveBitvector(stream, bitvector1);
    }
    else {
        stream << (unsigned char)2;
        saveBitvector(stream, bitvector2);
    }
}

void decompressColor(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B);
void decompressGrayscale(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& map);

void decompress(const char * infile, const char * outfile) {
    std::ifstream file(infile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "erreur : Impossible de lire l'image compresse" << std::endl;
        exit(0);
    }
    IStreamer stream(file);

    ImagePPM imOut;
    unsigned int width, height;
    stream >> width;
    stream >> height;
    char color;
    stream >> color;
    if (color == 1) {
        Bitmap<unsigned char> R, G, B;
        decompressColor(stream, width, height, R, G, B);
        imOut.setRed(R);
        imOut.setGreen(G);
        imOut.setBlue(B);
    }
    else {
        Bitmap<unsigned char> map;
        decompressGrayscale(stream, width, height, map);
        imOut = map;
    }
    file.close();

    if (!imOut.save(outfile)) {
        std::cerr << "erreur : Impossible d'ecrire l'image decompresse" << std::endl;
        exit(0);
    }
}

void loadBitvector(IStreamer& stream, std::vector<bool>& bitvector);

void decompressColor(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B) {
    Bitmap<unsigned char> YQ, Cr3, Cb3, YMeanQ, YDiffQ, YDiffQ2;
    Bitmap<float> Y, YMean, YDiff, Cr, Cr2, Cb, Cb2;
    std::vector<bool> bitvector;
    unsigned char c;
    stream >> c;
    loadBitvector(stream, bitvector);
    if (c == 1) {
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YMeanQ, width / 2, height, 3));
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, YMeanQ, width / 2, height, 7, 3, 16));
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YDiffQ, width / 2, height, 4));
        Process::invertGrayCoding(YMeanQ, YMeanQ);
        Process::Unquantify(YMeanQ, YMean, 7);
        Process::EnlargeQuantify(YDiffQ, YDiffQ2, 2);
        Process::LogUnquantify(YDiffQ2, YDiff, 6);
        Process::invertFilter(YMean, YDiff, Y);
    }
    else {
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YQ, width, height, 3));
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, YQ, width, height, 6, 3, 16));
        Process::Unquantify(YQ, Y, 6);
    }
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, Cr3, width / 2, height / 2, 2));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, Cr3, width / 2, height / 2, 7, 2, 16));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, Cb3, width / 2, height / 2, 2));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, Cb3, width / 2, height / 2, 7, 2, 16));
    Process::invertGrayCoding(Cr3, Cr3);
    Process::invertGrayCoding(Cb3, Cb3);
    Process::Unquantify(Cr3, Cr2, 7);
    Process::Unquantify(Cb3, Cb2, 7);
    Cr2.resize(Y.width() / 2, Y.height() / 2);
    Cb2.resize(Y.width() / 2, Y.height() / 2);
    Process::Enlarge2(Cr2, Cr);
    Process::Enlarge2(Cb2, Cb);
    Process::toRGB(Y, Cr, Cb, R, G, B);
}

void decompressGrayscale(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& map) {
    Bitmap<float> Y;
    Bitmap<unsigned char> YQ;
    std::vector<bool> bitvector;
    unsigned char c;
    stream >> c;
    loadBitvector(stream, bitvector);
    if (c == 1) {
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YQ, width, height, 3));
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, YQ, width, height, 6, 3, 16));
    }
    else {
        bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YQ, width, height, 6));
    }
    Process::Unquantify(YQ, Y, 6);
    map = Y;
}

void saveBitvector(OStreamer& stream, std::vector<bool>& bitvector) {
    unsigned char c = 0;
    for (unsigned long i = 0, size = bitvector.size(); i < size; i++) {
        c |= bitvector[i] << (i % 8);
        if ((i + 1) % 8 == 0) {
            stream << c;
            c = 0;
        }
    }
    if (bitvector.size() % 8 != 0)
        stream << c;
}

void loadBitvector(IStreamer& stream, std::vector<bool>& bitvector) {
    unsigned char c;
    for (unsigned long i = 0; !stream.eof(); i++) {
        if (i % 8 == 0)
            stream >> c;
        bitvector.push_back((c >> (i % 8)) & 0x1);
    }
}
