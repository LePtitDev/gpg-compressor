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
    Bitmap<float> Y, YMean, YDiff, Cr, Cr2, Cb, Cb2;
    Process::toYCrCb(R, G, B, Y, Cr, Cb);
    Process::filterMean(Y, YMean);
    Process::filterSub(Y, YDiff);
    Bitmap<unsigned char> YMeanQ, YDiffQ, YDiffQ2;
    Process::Quantify(YMean, YMeanQ, 7);
    Process::LogQuantify(YDiff, YDiffQ2, 6);
    Process::ReduceQuantify(YDiffQ2, YDiffQ, 2);
    Process::Reduce2(Cr, Cr2);
    Process::Reduce2(Cb, Cb2);
    Bitmap<unsigned char> G2, B2;
    Process::Quantify(Cr2, G2, 7);
    Process::Quantify(Cb2, B2, 7);
    Process::grayCoding(YMeanQ, YMeanQ);
    std::vector<bool> bitvector;
    std::cout << Process::huffman(YMeanQ, bitvector, 3) << std::endl;
    std::cout << Process::arithmeticEncoding(YMeanQ, bitvector, 7, 3, 16) << std::endl;
    std::cout << Process::huffman(YDiffQ, bitvector, 6) << std::endl;
    std::cout << Process::huffman(G2, bitvector, 7) << std::endl;
    std::cout << Process::huffman(B2, bitvector, 7) << std::endl;
    saveBitvector(stream, bitvector);
}

void compressGrayscale(OStreamer& stream, const Bitmap<unsigned char>& map) {
    Bitmap<float> Y, YMean, YDiff;
    Y = map;
    Process::filterMean(Y, YMean);
    Process::filterSub(Y, YDiff);
    Bitmap<unsigned char> YMeanQ, YDiffQ, YDiffQ2;
    Process::Quantify(YMean, YMeanQ, 7);
    Process::LogQuantify(YDiff, YDiffQ2, 6);
    Process::ReduceQuantify(YDiffQ2, YDiffQ, 2);
    Process::grayCoding(YMeanQ, YMeanQ);
    std::vector<bool> bitvector;
    Process::huffman(YMeanQ, bitvector, 3);
    Process::arithmeticEncoding(YMeanQ, bitvector, 7, 3, 16);
    Process::huffman(YDiffQ, bitvector);
    saveBitvector(stream, bitvector);
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
        std::cerr << "erreur : Impossible d'ecrire l'image' decompresse" << std::endl;
        exit(0);
    }
}

void loadBitvector(IStreamer& stream, std::vector<bool>& bitvector);

void decompressColor(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B) {
    Bitmap<float> Y, YMean, YDiff, Cr, Cr2, Cb, Cb2;
    Bitmap<unsigned char> Cr3, Cb3, YMeanQ, YDiffQ, YDiffQ2;
    std::vector<bool> bitvector;
    loadBitvector(stream, bitvector);
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YMeanQ, width / 2, height, 3));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, YMeanQ, width / 2, height, 7, 3, 16));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YDiffQ, width / 2, height, 6));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, Cr3, width / 2, height / 2, 7));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, Cb3, width / 2, height / 2, 7));
    Process::invertGrayCoding(YMeanQ, YMeanQ);
    Process::Unquantify(YMeanQ, YMean, 7);
    Process::EnlargeQuantify(YDiffQ, YDiffQ2, 2);
    Process::LogUnquantify(YDiffQ2, YDiff, 6);
    Process::invertFilter(YMean, YDiff, Y);
    Process::Unquantify(Cr3, Cr2, 7);
    Process::Unquantify(Cb3, Cb2, 7);
    Cr2.resize(Y.width() / 2, Y.height() / 2);
    Cb2.resize(Y.width() / 2, Y.height() / 2);
    Process::Enlarge2(Cr2, Cr);
    Process::Enlarge2(Cb2, Cb);
    Process::toRGB(Y, Cr, Cb, R, G, B);
}

void decompressGrayscale(IStreamer& stream, unsigned int width, unsigned int height, Bitmap<unsigned char>& map) {
    Bitmap<float> Y, YMean, YDiff;
    Bitmap<unsigned char> YMeanQ, YDiffQ, YDiffQ2;
    std::vector<bool> bitvector;
    loadBitvector(stream, bitvector);
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YMeanQ, width / 2, height, 3));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertArithmeticEncoding(bitvector, YMeanQ, width / 2, height, 7, 3, 16));
    bitvector.erase(bitvector.begin(), bitvector.begin() + Process::invertHuffman(bitvector, YDiffQ, width / 2, height));
    Process::invertGrayCoding(YMeanQ, YMeanQ);
    Process::Unquantify(YMeanQ, YMean, 7);
    Process::EnlargeQuantify(YDiffQ, YDiffQ2, 2);
    Process::LogUnquantify(YDiffQ2, YDiff, 6);
    Process::invertFilter(YMean, YDiff, Y);
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
