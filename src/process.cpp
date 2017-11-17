#include "process.h"
#include "huffman.h"

#include <cmath>
#include <array>
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

void Process::LogQuantify2(const Bitmap<float>& in, Bitmap<unsigned char>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    float count = (float)(1 << (N - 1));
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            if (in[i][j] > 128.0f)
                out[i][j] = (unsigned int)(64.0f * (std::log2(in[i][j] - 128.0f) + 1.0f) / count + count);
            else if (in[i][j] < 128.0f)
                out[i][j] = (unsigned int)(count - 64.0f * (std::log2(128.0f - in[i][j]) + 1.0f) / count);
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

void Process::LogUnquantify2(const Bitmap<unsigned char>& in, Bitmap<float>& out, unsigned int N) {
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
                out[i][j] = 128.0f + 2.0f * std::pow(2.0f, ((float)(in[i][j] - C)) * den);
            else
                out[i][j] = 128.0f - 2.0f * std::pow(2.0f, ((float)(C - in[i][j])) * den);
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

unsigned int Process::huffman(const Bitmap<unsigned char>& in, std::vector<bool>& out, unsigned int N) {
    Huffman<8> huff;
    huff.create(in.data(), in.width() * in.height(), N);
    unsigned int it = huff.write(out, N);
    return it + huff.write(out, in.data(), in.width() * in.height());
}

unsigned int Process::invertHuffman(const std::vector<bool>& in, Bitmap<unsigned char>& out, unsigned int width, unsigned int height, unsigned int N) {
    if (out.width() != width || out.height() != height)
        out.resize(width, height);
    Huffman<8> huff;
    unsigned int it = huff.read(in, N);
    std::vector<bool> dta (in.begin() + it, in.end());
    return it + huff.read(dta, out.data(), width * height);
}

namespace Process {

    std::array<unsigned char, 256> grayTable {
        0b00000000, 0b00000001, 0b00000011, 0b00000010,
        0b00000110, 0b00000111, 0b00000101, 0b00000100,
        0b00001100, 0b00001101, 0b00001111, 0b00001110,
        0b00001010, 0b00001011, 0b00001001, 0b00001000,

        0b00011000, 0b00011001, 0b00011011, 0b00011010,
        0b00011110, 0b00011111, 0b00011101, 0b00011100,
        0b00010100, 0b00010101, 0b00010111, 0b00010110,
        0b00010010, 0b00010011, 0b00010001, 0b00010000,

        0b00110000, 0b00110001, 0b00110011, 0b00110010,
        0b00110110, 0b00110111, 0b00110101, 0b00110100,
        0b00111100, 0b00111101, 0b00111111, 0b00111110,
        0b00111010, 0b00111011, 0b00111001, 0b00111000,

        0b00101000, 0b00101001, 0b00101011, 0b00101010,
        0b00101110, 0b00101111, 0b00101101, 0b00101100,
        0b00100100, 0b00100101, 0b00100111, 0b00100110,
        0b00100010, 0b00100011, 0b00100001, 0b00100000,

        0b01100000, 0b01100001, 0b01100011, 0b01100010,
        0b01100110, 0b01100111, 0b01100101, 0b01100100,
        0b01101100, 0b01101101, 0b01101111, 0b01101110,
        0b01101010, 0b01101011, 0b01101001, 0b01101000,

        0b01111000, 0b01111001, 0b01111011, 0b01111010,
        0b01111110, 0b01111111, 0b01111101, 0b01111100,
        0b01110100, 0b01110101, 0b01110111, 0b01110110,
        0b01110010, 0b01110011, 0b01110001, 0b01110000,

        0b01010000, 0b01010001, 0b01010011, 0b01010010,
        0b01010110, 0b01010111, 0b01010101, 0b01010100,
        0b01011100, 0b01011101, 0b01011111, 0b01011110,
        0b01011010, 0b01011011, 0b01011001, 0b01011000,

        0b01001000, 0b01001001, 0b01001011, 0b01001010,
        0b01001110, 0b01001111, 0b01001101, 0b01001100,
        0b01000100, 0b01000101, 0b01000111, 0b01000110,
        0b01000010, 0b01000011, 0b01000001, 0b01000000,

        0b11000000, 0b11000001, 0b11000011, 0b11000010,
        0b11000110, 0b11000111, 0b11000101, 0b11000100,
        0b11001100, 0b11001101, 0b11001111, 0b11001110,
        0b11001010, 0b11001011, 0b11001001, 0b11001000,

        0b11011000, 0b11011001, 0b11011011, 0b11011010,
        0b11011110, 0b11011111, 0b11011101, 0b11011100,
        0b11010100, 0b11010101, 0b11010111, 0b11010110,
        0b11010010, 0b11010011, 0b11010001, 0b11010000,

        0b11110000, 0b11110001, 0b11110011, 0b11110010,
        0b11110110, 0b11110111, 0b11110101, 0b11110100,
        0b11111100, 0b11111101, 0b11111111, 0b11111110,
        0b11111010, 0b11111011, 0b11111001, 0b11111000,

        0b11101000, 0b11101001, 0b11101011, 0b11101010,
        0b11101110, 0b11101111, 0b11101101, 0b11101100,
        0b11100100, 0b11100101, 0b11100111, 0b11100110,
        0b11100010, 0b11100011, 0b11100001, 0b11100000,

        0b10100000, 0b10100001, 0b10100011, 0b10100010,
        0b10100110, 0b10100111, 0b10100101, 0b10100100,
        0b10101100, 0b10101101, 0b10101111, 0b10101110,
        0b10101010, 0b10101011, 0b10101001, 0b10101000,

        0b10111000, 0b10111001, 0b10111011, 0b10111010,
        0b10111110, 0b10111111, 0b10111101, 0b10111100,
        0b10110100, 0b10110101, 0b10110111, 0b10110110,
        0b10110010, 0b10110011, 0b10110001, 0b10110000,

        0b10010000, 0b10010001, 0b10010011, 0b10010010,
        0b10010110, 0b10010111, 0b10010101, 0b10010100,
        0b10011100, 0b10011101, 0b10011111, 0b10011110,
        0b10011010, 0b10011011, 0b10011001, 0b10011000,

        0b10001000, 0b10001001, 0b10001011, 0b10001010,
        0b10001110, 0b10001111, 0b10001101, 0b10001100,
        0b10000100, 0b10000101, 0b10000111, 0b10000110,
        0b10000010, 0b10000011, 0b10000001, 0b10000000
    };

    std::array<unsigned char, 256> invertGrayTable {
        0b00000000, 0b00000001, 0b00000011, 0b00000010,
        0b00000111, 0b00000110, 0b00000100, 0b00000101,
        0b00001111, 0b00001110, 0b00001100, 0b00001101,
        0b00001000, 0b00001001, 0b00001011, 0b00001010,

        0b00011111, 0b00011110, 0b00011100, 0b00011101,
        0b00011000, 0b00011001, 0b00011011, 0b00011010,
        0b00010000, 0b00010001, 0b00010011, 0b00010010,
        0b00010111, 0b00010110, 0b00010100, 0b00010101,

        0b00111111, 0b00111110, 0b00111100, 0b00111101,
        0b00111000, 0b00111001, 0b00111011, 0b00111010,
        0b00110000, 0b00110001, 0b00110011, 0b00110010,
        0b00110111, 0b00110110, 0b00110100, 0b00110101,

        0b00100000, 0b00100001, 0b00100011, 0b00100010,
        0b00100111, 0b00100110, 0b00100100, 0b00100101,
        0b00101111, 0b00101110, 0b00101100, 0b00101101,
        0b00101000, 0b00101001, 0b00101011, 0b00101010,

        0b01111111, 0b01111110, 0b01111100, 0b01111101,
        0b01111000, 0b01111001, 0b01111011, 0b01111010,
        0b01110000, 0b01110001, 0b01110011, 0b01110010,
        0b01110111, 0b01110110, 0b01110100, 0b01110101,

        0b01100000, 0b01100001, 0b01100011, 0b01100010,
        0b01100111, 0b01100110, 0b01100100, 0b01100101,
        0b01101111, 0b01101110, 0b01101100, 0b01101101,
        0b01101000, 0b01101001, 0b01101011, 0b01101010,

        0b01000000, 0b01000001, 0b01000011, 0b01000010,
        0b01000111, 0b01000110, 0b01000100, 0b01000101,
        0b01001111, 0b01001110, 0b01001100, 0b01001101,
        0b01001000, 0b01001001, 0b01001011, 0b01001010,

        0b01011111, 0b01011110, 0b01011100, 0b01011101,
        0b01011000, 0b01011001, 0b01011011, 0b01011010,
        0b01010000, 0b01010001, 0b01010011, 0b01010010,
        0b01010111, 0b01010110, 0b01010100, 0b01010101,

        0b11111111, 0b11111110, 0b11111100, 0b11111101,
        0b11111000, 0b11111001, 0b11111011, 0b11111010,
        0b11110000, 0b11110001, 0b11110011, 0b11110010,
        0b11110111, 0b11110110, 0b11110100, 0b11110101,

        0b11100000, 0b11100001, 0b11100011, 0b11100010,
        0b11100111, 0b11100110, 0b11100100, 0b11100101,
        0b11101111, 0b11101110, 0b11101100, 0b11101101,
        0b11101000, 0b11101001, 0b11101011, 0b11101010,

        0b11000000, 0b11000001, 0b11000011, 0b11000010,
        0b11000111, 0b11000110, 0b11000100, 0b11000101,
        0b11001111, 0b11001110, 0b11001100, 0b11001101,
        0b11001000, 0b11001001, 0b11001011, 0b11001010,

        0b11011111, 0b11011110, 0b11011100, 0b11011101,
        0b11011000, 0b11011001, 0b11011011, 0b11011010,
        0b11010000, 0b11010001, 0b11010011, 0b11010010,
        0b11010111, 0b11010110, 0b11010100, 0b11010101,

        0b10000000, 0b10000001, 0b10000011, 0b10000010,
        0b10000111, 0b10000110, 0b10000100, 0b10000101,
        0b10001111, 0b10001110, 0b10001100, 0b10001101,
        0b10001000, 0b10001001, 0b10001011, 0b10001010,

        0b10011111, 0b10011110, 0b10011100, 0b10011101,
        0b10011000, 0b10011001, 0b10011011, 0b10011010,
        0b10010000, 0b10010001, 0b10010011, 0b10010010,
        0b10010111, 0b10010110, 0b10010100, 0b10010101,

        0b10111111, 0b10111110, 0b10111100, 0b10111101,
        0b10111000, 0b10111001, 0b10111011, 0b10111010,
        0b10110000, 0b10110001, 0b10110011, 0b10110010,
        0b10110111, 0b10110110, 0b10110100, 0b10110101,

        0b10100000, 0b10100001, 0b10100011, 0b10100010,
        0b10100111, 0b10100110, 0b10100100, 0b10100101,
        0b10101111, 0b10101110, 0b10101100, 0b10101101,
        0b10101000, 0b10101001, 0b10101011, 0b10101010
    };

}

void Process::grayCoding(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] = grayTable[in[i][j]];
        }
    }
}

void Process::invertGrayCoding(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] = invertGrayTable[in[i][j]];
        }
    }
}

void Process::getBinary(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] = (in[i][j] >> N) & 0x1;
        }
    }
}

void Process::setBinary(const Bitmap<unsigned char>& in, Bitmap<unsigned char>& out, unsigned int N) {
    if (in.width() != out.width() || in.height() != out.height())
        out.resize(in.width(), in.height());
    for (unsigned int i = 0, h = in.height(); i < h; i++) {
        for (unsigned int j = 0, w = in.width(); j < w; j++) {
            out[i][j] &= ~(0x1 << N);
            out[i][j] |= ~(0x1 << N) & (in[i][j] << N);
        }
    }
}