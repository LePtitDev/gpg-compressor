#include "process.h"

#include <cmath>
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

void Process::toYCrCb(const Bitmap<unsigned char>& R, const Bitmap<unsigned char>& G, const Bitmap<unsigned char>& B, Bitmap<float>& Y, Bitmap<float>& Cr, Bitmap<float>& Cb) {
    if (R.width() != Y.width() || R.height() != Y.height())
        Y.resize(R.width(), R.height());
    if (R.width() != Cr.width() || R.height() != Cr.height())
        Cr.resize(R.width(), R.height());
    if (R.width() != Cb.width() || R.height() != Cb.height())
        Cb.resize(R.width(), R.height());
    for (unsigned int i = 0, h = R.height(); i < h; i++) {
        for (unsigned int j = 0, w = R.width(); j < w; j++) {
            Y[i][j]  = std::max(0.0f, std::min(255.0f, (float)R[i][j] * 0.299f + (float)G[i][j] * 0.587f + (float)B[i][j] * 0.114f));
            Cr[i][j] = std::max(0.0f, std::min(255.0f, (float)R[i][j] * 0.500f - (float)G[i][j] * 0.4187f - (float)B[i][j] * 0.0813f + 128.0f));
            Cb[i][j] = std::max(0.0f, std::min(255.0f, -(float)R[i][j] * 0.1687f - (float)G[i][j] * 0.3313f + (float)B[i][j] * 0.500f + 128.0f));
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
            R[i][j] = std::max(0.0f, std::min(255.0f, 1.0000f * Y[i][j] + 1.402f * (Cr[i][j] - 128.0f) + 0.0000f * (Cb[i][j] - 128.0f)));
            G[i][j] = std::max(0.0f, std::min(255.0f, 1.0000f * Y[i][j] - 0.71414f * (Cr[i][j] - 128.0f) - 0.34414f * (Cb[i][j] - 128.0f)));
            B[i][j] = std::max(0.0f, std::min(255.0f, 1.0000f * Y[i][j] + 0.0000f * (Cr[i][j] - 128.0f) + 1.772f * (Cb[i][j] - 128.0f)));
        }
    }
}

float Process::calculatePSNR(const Bitmap<unsigned char>& first, const Bitmap<unsigned char>& second) {
    if (first.width() != second.width() || first.height() != second.height())
        return 0.0f;
    float eqm = 0.0f;
    for (unsigned int i = 0, h = first.height(); i < h; i++) {
        for (unsigned int j = 0, w = second.width(); j < w; j++)
            eqm += std::pow((float)first[i][j] - (float)second[i][j], 2.0f);
    }
    eqm /= (float)(first.width() * first.height());
    return 10.0f * std::log10((float)(255 * 255) / eqm);
}

void Process::Reduce2(const Bitmap<float>& in, Bitmap<float>& out) {
    unsigned int w = (in.width() + 1) / 2, w2 = in.width(),
                 h = (in.height() + 1) / 2, h2 = in.height();
    if (in.width() != w || in.height() != h)
        out.resize(w, h);
    for (unsigned int i = 0; i < h; i++) {
        for (unsigned int j = 0; j < w; j++) {
            bool i21 = (i * 2 + 1 < h2), j21 = (j * 2 + 1 < w2);
            float tx = 1.0f / ((i21 ? 2.0f : 1.0f) * (j21 ? 2.0f : 1.0f));
            out[i][j] = tx * (float)in[i * 2][j * 2] +
                        tx * (j21 ? (float)in[i * 2][j * 2 + 1] : 0.0f) +
                        tx * (i21 ? (float)in[i * 2 + 1][j * 2] : 0.0f) +
                        tx * (i21 && j21 ? (float)in[i * 2 + 1][j * 2 + 1] : 0.0f);
        }
    }
}

void Process::Enlarge2(const Bitmap<float>& in, Bitmap<float>& out) {
    if (in.width() * 2 != out.width() || in.height() * 2 != out.height())
        out.resize(in.width() * 2, in.height() * 2);
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i * 2][j * 2] = in[i][j];
            out[i * 2][j * 2 + 1] = in[i][j];
            out[i * 2 + 1][j * 2] = in[i][j];
            out[i * 2 + 1][j * 2 + 1] = in[i][j];
        }
    }
}

void Process::Quantify(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] = (unsigned char)in[i][j] >> (8 - N);
        }
    }
}

void Process::Unquantify(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] = (float)(in[i][j] << (8 - N));
        }
    }
}

void Process::LogQuantify(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    float count = (float)(1 << (N - 1));
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            if (in[i][j] > 128.0f)
                out[i][j] = (unsigned int)(128.0f * (std::log2(in[i][j] - 128.0f) + 1.0f) / count + count);
            else if (in[i][j] < 128.0f)
                out[i][j] = (unsigned int)(count - 128.0f * (std::log2(128.0f - in[i][j]) + 1.0f) / count);
            else
                out[i][j] = (unsigned int)count;
        }
    }
}

void Process::LogUnquantify(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    float count = (float)(1 << (N - 1));
    unsigned char C = (1 << (N - 1));
    float den = 7.0f / count;
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            if (in[i][j] == C)
                out[i][j] = 128.0f;
            else if (in[i][j] > C)
                out[i][j] = 128.0f + std::pow(2.0f, ((float)(in[i][j] - C)) * den);
            else
                out[i][j] = 128.0f - std::pow(2.0f, ((float)(C - in[i][j])) * den);
        }
    }
}

void Process::filterSub(const Bitmap<float>& in, Bitmap<float>& out) {
    if (in.width() / 2 != out.width() || in.height() != out.height())
        out.resize(in.width() / 2, in.height());
    for (unsigned int i = 0, h = out.height(); i < h; i++) {
        for (unsigned int j = 0, w = out.width(); j < w; j++) {
            out[i][j] = in[i][j * 2 + 1] - in[i][j * 2] + 128.0f;
        }
    }
}

void Process::filterUp(const Bitmap<float>& in, Bitmap<float>& out) {
    if (in.width() != out.width() || in.height() / 2 != out.height())
        out.resize(in.width(), in.height() / 2);
    for (unsigned int i = 0, h = out.height(); i < h; i++) {
        for (unsigned int j = 0, w = out.width(); j < w; j++) {
            out[i][j] = in[i * 2 + 1][j] - in[i * 2][j] + 128.0f;
        }
    }
}

void Process::filterMean(const Bitmap<float>& in, Bitmap<float>& out) {
    if (in.width() / 2 != out.width() || in.height() != out.height())
        out.resize(in.width() / 2, in.height());
    for (unsigned int i = 0, h = out.height(); i < h; i++) {
        for (unsigned int j = 0, w = out.width(); j < w; j++) {
            out[i][j] = 0.5f * in[i][j * 2] + 0.5f * in[i][j * 2 + 1];
        }
    }
}

void Process::filterMeanUp(const Bitmap<float>& in, Bitmap<float>& out) {
    if (in.width() != out.width() || in.height() / 2 != out.height())
        out.resize(in.width(), in.height() / 2);
    for (unsigned int i = 0, h = out.height(); i < h; i++) {
        for (unsigned int j = 0, w = out.width(); j < w; j++) {
            out[i][j] = 0.5f * in[i * 2][j] + 0.5f * in[i * 2 + 1][j];
        }
    }
}

void Process::invertFilter(const Bitmap<float>& mean, const Bitmap<float>& sub, Bitmap<float>& out) {
    if (mean.width() * 2 != out.width() || mean.height() != out.height())
        out.resize(mean.width() * 2, mean.height());
    for (unsigned int i = 0, h = out.height(); i < h; i++) {
        for (unsigned int j = 0, w = mean.width(); j < w; j++) {
            out[i][j * 2] = mean[i][j] - (sub[i][j] - 128.0f) / 2.0f;
            out[i][j * 2 + 1] = mean[i][j] + (sub[i][j] - 128.0f) / 2.0f;
        }
    }
}

void Process::invertFilterUp(const Bitmap<float>& mean, const Bitmap<float>& up, Bitmap<float>& out) {
    if (mean.width() != out.width() || mean.height() * 2 != out.height())
        out.resize(mean.width(), mean.height() * 2);
    for (unsigned int i = 0, h = mean.height(); i < h; i++) {
        for (unsigned int j = 0, w = out.width(); j < w; j++) {
            out[i * 2][j] = mean[i][j] - (up[i][j] - 128.0f) / 2.0f;
            out[i * 2 + 1][j] = mean[i][j] + (up[i][j] - 128.0f) / 2.0f;
        }
    }
}