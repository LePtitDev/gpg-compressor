#ifndef PROCESS_H
#define PROCESS_H

#include "bitmap.h"

#include <vector>

namespace Process {

    void toGrayscale(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B, Bitmap<unsigned char>& Y);

    void toYCrCb(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B,
                 Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb);
    
    void toRGB(const Bitmap<float>& Y, const Bitmap<float>& Cr, const Bitmap<float>& Cb,
               Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B);
    
    float calculatePSNR(const Bitmap<unsigned char>& first, const Bitmap<unsigned char>& second);

    void Reduce2(const Bitmap<float>& in, Bitmap<float>& out);
    
    void Enlarge2(const Bitmap<float>& in, Bitmap<float>& out);

    void Quantify(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N);

    void ReduceQuantify(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N);
    
    void Unquantify(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N);
    
    void EnlargeQuantify(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N);

    void LogQuantify(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N);

    void LogQuantify2(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N);

    void LogUnquantify(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N);

    void LogUnquantify2(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N);

    void filterSub(const Bitmap<float>& in, Bitmap<float>& out);
    
    void filterUp(const Bitmap<float>& in, Bitmap<float>& out);

    void filterMean(const Bitmap<float>& in, Bitmap<float>& out);
    
    void filterMeanUp(const Bitmap<float>& in, Bitmap<float>& out);

    void invertFilter(const Bitmap<float>& mean, const Bitmap<float>& sub, Bitmap<float>& out);
    
    void invertFilterUp(const Bitmap<float>& mean, const Bitmap<float>& sub, Bitmap<float>& out);

    unsigned int huffman(const Bitmap<unsigned char>& in, std::vector<bool>& out, unsigned int N = 8);

    unsigned int invertHuffman(const std::vector<bool>& in, Bitmap<unsigned char>& out, unsigned int width, unsigned int height, unsigned int N = 8);

    void grayCoding(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out);

    void invertGrayCoding(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out);

    void getBinary(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N = 0);

    void setBinary(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N = 0);

}

#endif // PROCESS_H