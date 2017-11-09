#ifndef PROCESS_H
#define PROCESS_H

#include "bitmap.h"

namespace Process {

    void toGrayscale(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B, Bitmap<unsigned char>& Y);

    void toYCrCb(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B,
                 Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb);
    
    void toRGB(const Bitmap<float>& Y, const Bitmap<float>& Cr, const Bitmap<float>& Cb,
               Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B);

}

#endif // PROCESS_H