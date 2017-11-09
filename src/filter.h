#ifndef FILTER_H
#define FILTER_H

#include "bitmap.h"

namespace Filter {

    template <typename T>
    void sub(const Bitmap<T>& in, Bitmap<T>& out) {
        if (in.width() / 2 != out.width() || in.height() != out.height())
            out.resize(in.width() / 2, in.height());
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = (float)in[i][j * 2 + 1] - (float)in[i][j * 2] + 128.0f;
            }
        }
    }
    template <>
    void sub(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out) {
        if (in.width() / 2 != out.width() || in.height() != out.height())
            out.resize(in.width() / 2, in.height());
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = (int)in[i][j * 2 + 1] - (int)in[i][j * 2] + 128;
            }
        }
    }
    
    template <typename T>
    void up(const Bitmap<T>& in, Bitmap<T>& out) {
        if (in.width() != out.width() || in.height() / 2 != out.height())
            out.resize(in.width(), in.height() / 2);
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = (float)in[i * 2 + 1][j] - (float)in[i * 2][j] + 128.0f;
            }
        }
    }
    template <>
    void up(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out) {
        if (in.width() != out.width() || in.height() / 2 != out.height())
            out.resize(in.width(), in.height() / 2);
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = (int)in[i * 2 + 1][j] - (int)in[i * 2][j] + 128;
            }
        }
    }

    template <typename T>
    void mean(const Bitmap<T>& in, Bitmap<T>& out) {
        if (in.width() / 2 != out.width() || in.height() != out.height())
            out.resize(in.width() / 2, in.height());
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = 0.5f * (float)in[i][j * 2] + 0.5f * (float)in[i][j * 2 + 1];
            }
        }
    }
    
    template <typename T>
    void meanUp(const Bitmap<T>& in, Bitmap<T>& out) {
        if (in.width() != out.width() || in.height() / 2 != out.height())
            out.resize(in.width(), in.height() / 2);
        for (unsigned int i = 0, h = out.height(); i < h; i++) {
            for (unsigned int j = 0, w = out.width(); j < w; j++) {
                out[i][j] = 0.5f * (float)in[i * 2][j] + 0.5f * (float)in[i * 2 + 1][j];
            }
        }
    }

}

#endif // FILTER_H