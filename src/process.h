#ifndef PROCESS_H
#define PROCESS_H

#include "bitmap.h"

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
    
    void Unquantify(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N);

    void filterSub(const Bitmap<float>& in, Bitmap<float>& out);
    
    void filterUp(const Bitmap<float>& in, Bitmap<float>& out);

    void filterMean(const Bitmap<float>& in, Bitmap<float>& out);
    
    void filterMeanUp(const Bitmap<float>& in, Bitmap<float>& out);

}

#endif // PROCESS_H