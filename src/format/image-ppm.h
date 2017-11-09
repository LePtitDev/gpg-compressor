#ifndef IMAGE_PPM_H
#define IMAGE_PPM_H

#include "../image.h"

class ImagePPM : public Image {

public:

    ImagePPM() {}
    ImagePPM(unsigned int width, unsigned int height, bool color = true) : Image(width, height, color) {}
    ImagePPM(const Bitmap<PixelRGB>& map) : Image(map) {}

    bool load(const char * filename);
    bool save(const char * filename);

};

#endif // IMAGE_PPM_H