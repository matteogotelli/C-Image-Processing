//
// Created by Matteo on 10/02/2026.
//

#ifndef IMAGE_PROCESSING_IMAGEG_HPP
#define IMAGE_PROCESSING_IMAGEG_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Kernel.hpp"

#include "stb_image.h"
#include "stb_image_write.h"

class ImageG {

    int channels = 3, width = 0, height = 0;
    std::vector<unsigned char> data;
    std::string name;

    typedef std::vector<float> values;

public:
    ImageG() = default;
    explicit ImageG(const std::string& filename);
    explicit ImageG(int c, int w, int h, std::string n = "");

    std::string getName() const {return name;}

    int sub2index(int x, int y) const;
    int sub2index(char ch, int x, int y) const;
    int sub2index(int ch, int x, int y) const;

    unsigned char operator () (char ch, int x, int y) const;
    unsigned char operator () (int ch, int x, int y) const;
    unsigned char & operator () (char ch, int x, int y);
    unsigned char & operator () (int ch, int x, int y);

    ImageG bn() const;
    ImageG crop(int cx, int cy, int w, int h);
    bool equal(const ImageG& b) const;
    float avg_diff(const ImageG& b) const;
    values conv_raw(const Kernel & k) const;
    ImageG conv(const Kernel & k) const;

    ImageG operator+(const ImageG& b) const;
    ImageG operator-(const ImageG& b) const;
    ImageG operator*(const ImageG& b) const;
    ImageG abs_diff(const ImageG& b) const;
    ImageG inv() const;

    ImageG bin_blur(int d) const;
    ImageG f_bin_blur(int d) const;

    std::pair<ImageG, values> grad() const;
    values laplacian() const;
    values LoG(int d) const;
    ImageG zc_LoG(int d, float threshold) const;

    void save(const std::string& filename);
};

#endif //IMAGE_PROCESSING_IMAGEG_HPP