#include "image-ppm.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>

namespace FormatPPM {

    bool write_ppm(const char * filename, const unsigned char * data, int width, int height);
    bool read_ppm(const char * filename, unsigned char ** data, int * width, int * height);
    bool write_pgm(const char * filename, const unsigned char * data, int width, int height);
    bool read_pgm(const char * filename, unsigned char ** data, int * width, int * height);

}

bool ImagePPM::load(const char * filename) {
    int len = std::string(filename).size();
    if (len < 4)
        return false;
    int width, height;
    unsigned char * dta;
    std::string ext(filename + len - 4);
    bool retry = true;
    if (ext == ".ppm")
        goto ppm_flag;
    else if (ext == ".pgm")
        goto pgm_flag;
    else {
        std::cerr << "Extension du fichier " << filename << " incorrecte" << std::endl;
        return false;
    }
ppm_flag:
    if (!FormatPPM::read_ppm(filename, &dta, &width, &height)) {
        if (retry) {
            retry = false;
            goto pgm_flag;
        }
        else
            return false;
    }
    resize(width, height, 0, 0, false);
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            at(i, j).r = dta[i * width * 3 + j * 3];
            at(i, j).g = dta[i * width * 3 + j * 3 + 1];
            at(i, j).b = dta[i * width * 3 + j * 3 + 2];
        }
    }
    color = true;
    goto ret_flag;
pgm_flag:
    if (!FormatPPM::read_pgm(filename, &dta, &width, &height)) {
        if (retry) {
            retry = false;
            goto ppm_flag;
        }
        else
            return false;
    }
    resize(width, height, 0, 0, false);
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            at(i, j).r = dta[i * width + j];
        }
    }
    color = false;
ret_flag:
    delete[] dta;
    return true;
}

bool ImagePPM::save(const char * filename) {
    bool result;
    unsigned char * dta = new unsigned char[width() * height() * (color ? 3 : 1)];
    if (color) {
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = width(); j < w; j++) {
                dta[i * w * 3 + j * 3] = at(i, j).r;
                dta[i * w * 3 + j * 3 + 1] = at(i, j).g;
                dta[i * w * 3 + j * 3 + 2] = at(i, j).b;
            }
        }
        result = FormatPPM::write_ppm(filename, (const unsigned char *)dta, (int)width(), (int)height());
    }
    else {
        for (unsigned int i = 0, h = height(); i < h; i++) {
            for (unsigned int j = 0, w = width(); j < w; j++) {
                dta[i * w + j] = at(i, j).r;
            }
        }
        result = FormatPPM::write_pgm(filename, (const unsigned char *)dta, (int)width(), (int)height());
    }
    delete[] dta;
    return result;
}

namespace FormatPPM {

    unsigned char ignore_comments(std::istream& file) {
        unsigned char c;
        while((c = (unsigned char)file.get()) == '#')
            while((c = (unsigned char)file.get()) != '\n');
        return c;
    }

    bool write_ppm(const char * filename, const unsigned char * data, int width, int height) {
        int taille_image = 3 * width * height;

        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Pas d'acces en ecriture sur l'image " << filename << std::endl;
            return false;
        }
        else {
            // Ecriture en-tête
            file << "P6\r";
            std::stringstream ss;
            ss << width << " " << height << "\r255\r";
            file << ss.str();

            if (!file.write((const char *)data, taille_image)) {
                std::cerr << "Erreur d'ecriture de l'image " << filename << std::endl;
                return false;
            }
            file.close();
        }
        return true;
    }
    
    bool read_ppm(const char * filename, unsigned char ** data, int * width, int * height) {
        int max_grey_val;
        
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Pas d'acces en lecture sur l'image " << filename << std::endl;
            return false;
        }
        else {
            // "P6 "
            file.get();
            if (file.get() != '6')
                return false;
            file.get();
            std::string stmp;
            stmp += ignore_comments(file);
            char c;
            if ((c = file.get()) != '\n')
                stmp += c;
            while ((c = file.get()) != '\r' && c != '\n' && c != ' ') stmp += c;
            *width = std::atoi(stmp.c_str());
            stmp.clear();
            if ((c = file.get()) != '\n')
                stmp += c;
            while ((c = file.get()) != '\r' && c != '\n' && c != ' ') stmp += c;
            *height = std::atoi(stmp.c_str());
            stmp.clear();
            if ((c = file.get()) != '\n')
                stmp += c;
            while ((c = file.get()) != '\r' && c != '\n' && c != ' ') stmp += c;
            max_grey_val = std::atoi(stmp.c_str());

            int taille_image = 3 * (*width) * (*height);
            *data = new unsigned char[taille_image];
            if (!file.read((char *)*data, taille_image)) {
                delete[] data;
                std::cerr << "Erreur de lecture de l'image " << filename << std::endl;
                return false;
            }
            file.close();
        }
        return true;
    }

    bool write_pgm(const char * filename, const unsigned char * data, int width, int height) {
        int taille_image = width * height;
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Pas d'acces en ecriture sur l'image " << filename << std::endl;
            return false;
        }
        else {
            // Ecriture en-tête
            file << "P5\r";
            std::stringstream ss;
            ss << width << " " << height << "\r255\r";
            file << ss.str();

            if (!file.write((const char *)data, taille_image)) {
                std::cerr << "Erreur d'ecriture de l'image " << filename << std::endl;
                return false;
            }
            file.close();
        }
        return true;
    }

    bool read_pgm(const char * filename, unsigned char ** data, int * width, int * height) {
        int max_grey_val;
        
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Pas d'acces en lecture sur l'image " << filename << std::endl;
            return false;
        }
        else {
            // "P5 "
            file.get();
            if (file.get() != '5')
                return false;
            file.get();
            std::string stmp;
            stmp += ignore_comments(file);
            char c;
            while ((c = file.get()) != '\r' && c != ' ') stmp += c;
            *width = std::atoi(stmp.c_str());
            stmp.clear();
            while ((c = file.get()) != '\r' && c != ' ') stmp += c;
            *height = std::atoi(stmp.c_str());
            stmp.clear();
            while ((c = file.get()) != '\r' && c != ' ') stmp += c;
            max_grey_val = std::atoi(stmp.c_str());

            int taille_image = (*width) * (*height);
            *data = new unsigned char[taille_image];
            if (!file.read((char *)*data, taille_image)) {
                delete[] data;
                std::cerr << "Erreur de lecture de l'image " << filename << std::endl;
                return false;
            }
            file.close();
        }
        return true;
    }

}