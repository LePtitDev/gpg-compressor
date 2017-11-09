#include "bitmap.h"

namespace Process {

    void toGrayscale(Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B, Bitmap<unsigned char>& Y);

    void toYCrCb(Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B,
                 Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb);
    
    void toRGB(Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb,
               Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B);

}