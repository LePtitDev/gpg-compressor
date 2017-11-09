#ifndef IMAGE_H
#define IMAGE_H

#include "bitmap.h"
#include "process.h"

typedef struct {
    unsigned char r, g, b;
    unsigned char& get(unsigned int i) {
        if (i == 1) return g;
        if (i == 2) return b;
        return r;
    }
    const unsigned char& get(unsigned int i) const {
        if (i == 1) return g;
        if (i == 2) return b;
        return r;
    }
    unsigned char& operator[](unsigned int i) { return get(i); }
    const unsigned char& operator[](unsigned int i) const { return get(i); }
} PixelRGB;

class Image : public Bitmap<PixelRGB> {

protected:

    bool color;

public:

    Image() {}
    Image(unsigned int width, unsigned int height, bool color = true) : Bitmap(width, height), color(color) {}
    Image(const Bitmap<PixelRGB>& map) : Bitmap(map) {}

    void colorize(bool c = true) { color = c; }
    bool colored() const { return color; }

    Bitmap<unsigned char> getColor(unsigned int c) const {
        Bitmap<unsigned char> res(width(), height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = width(); j < w; j++) {
                res[i][j] = at(i, j).get(c);
            }
        }
        return res;
    }
    
    Bitmap<unsigned char> getGrayscale() const { return getColor(0); }
    Bitmap<unsigned char> getRed() const { return getColor(0); }
    Bitmap<unsigned char> getGreen() const { return getColor(1); }
    Bitmap<unsigned char> getBlue() const { return getColor(2); }
    void setRed(const Bitmap<unsigned char>& map) {
        if (width() != map.width() || height() != map.height())
            resize(map.width(), map.height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = height(); j < w; j++) {
                at(i, j).r = map[i][j];
            }
        }
        color = true;
    }
    void setGreen(const Bitmap<unsigned char>& map) {
        if (width() != map.width() || height() != map.height())
            resize(map.width(), map.height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = height(); j < w; j++) {
                at(i, j).g = map[i][j];
            }
        }
        color = true;
    }
    void setBlue(const Bitmap<unsigned char>& map) {
        if (width() != map.width() || height() != map.height())
            resize(map.width(), map.height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = height(); j < w; j++) {
                at(i, j).b = map[i][j];
            }
        }
        color = true;
    }

    Image& operator=(const Bitmap<unsigned char>& map) {
        if (width() != map.width() || height() != map.height())
            resize(map.width(), map.height());
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = height(); j < w; j++) {
                at(i, j).r = map[i][j];
            }
        }
        color = false;
        return *this;
    }

};

#endif // IMAGE_H