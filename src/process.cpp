#include "process.h"

#include <algorithm>

void Process::toGrayscale(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B, Bitmap<unsigned char>& Y) {
    if (R.width() != Y.width() || R.height() != Y.height())
        Y.resize(R.width(), R.height());
    for (unsigned int i = 0, h = R.height(); i < h; i++) {
        for (unsigned int j = 0, w = R.width(); j < w; j++) {
            Y[i][j] = R[i][j] * 0.299f + G[i][j] * 0.587f + B[i][j] * 0.114f;
        }
    }
}

#include <iostream>
void Process::toYCrCb(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B, Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb) {
    if (R.width() != Y.width() || R.height() != Y.height())
        Y.resize(R.width(), R.height());
    if (R.width() != Cr.width() || R.height() != Cr.height())
        Cr.resize(R.width(), R.height());
    if (R.width() != Cb.width() || R.height() != Cb.height())
        Cb.resize(R.width(), R.height());
    for (unsigned int i = 0, h = R.height(); i < h; i++) {
        for (unsigned int j = 0, w = R.width(); j < w; j++) {
            Y[i][j] = std::max(0.0f, std::min(255.0f, (float)R[i][j] * 65.738f / 256.0f + (float)G[i][j] * 129.075f / 256.0f + (float)B[i][j] * 25.064f / 256.0f));
            Cr[i][j] = std::max(0.0f, std::min(255.0f, (float)R[i][j] * 112.439f / 256.0f - (float)G[i][j] * 94.154f / 256.0f - (float)B[i][j] * 18.285f / 256.0f + 128.0f));
            Cb[i][j] = std::max(0.0f, std::min(255.0f, -(float)R[i][j] * 37.945f / 256.0f - (float)G[i][j] * 74.494f / 256.0f + (float)B[i][j] * 112.439f / 256.0f + 128.0f));
        }
    }
}

void Process::toRGB(const Bitmap<float>& Y, const Bitmap<float>& Cr, const Bitmap<float>& Cb, Bitmap<unsigned char>& R, Bitmap<unsigned char>& G, Bitmap<unsigned char>& B) {
    if (Y.width() != R.width() || Y.height() != R.height())
        R.resize(Y.width(), Y.height());
    if (Y.width() != G.width() || Y.height() != G.height())
        G.resize(Y.width(), Y.height());
    if (Y.width() != B.width() || Y.height() != B.height())
        B.resize(Y.width(), Y.height());
    for (unsigned int i = 0, h = R.height(); i < h; i++) {
        for (unsigned int j = 0, w = R.width(); j < w; j++) {
            R[i][j] = std::max(0.0, std::min(255.0, 1.0000 * Y[i][j] + 1.4022 * (Cr[i][j] - 128.f) + 0.0000 * (Cb[i][j] - 128.f)));
            G[i][j] = std::max(0.0, std::min(255.0, 1.0000 * Y[i][j] - 0.7145 * (Cr[i][j] - 128.f) - 0.3456 * (Cb[i][j] - 128.f)));
            B[i][j] = std::max(0.0, std::min(255.0, 1.0000 * Y[i][j] + 0.0000 * (Cr[i][j] - 128.f) + 1.7710 * (Cb[i][j] - 128.f)));
        }
    }
}