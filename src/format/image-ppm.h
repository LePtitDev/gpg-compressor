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
    
    ImagePPM& operator=(const Bitmap<unsigned char>& map) {
        if (width() != map.width() || height() != map.height())
            resize(map.width(), map.height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = width(); j < w; j++) {
                at(i, j).r = map[i][j];
            }
        }
        color = false;
        return *this;
    }

};

#endif // IMAGE_PPM_H