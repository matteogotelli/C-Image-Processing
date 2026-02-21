//
// Created by Matteo on 10/02/2026.
//

#include "ImageG.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageG::ImageG(const std::string &filename) {

    unsigned char* temp_data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
    if (!temp_data) {
        throw std::runtime_error("Error loading image: ");
    }
    name = filename;
    std::cout << "Loading "<< name <<": " << width << "x" << height << " pixel." << std::endl;

    data.assign(temp_data, temp_data + width * height * 3);

    stbi_image_free(temp_data);


    std::cout << "Image "<< name <<" loaded: " << width << "x" << height << " pixels." << std::endl;
}

ImageG::ImageG(int c, int w, int h, std::string n) : channels(c), width(w), height(h), name(n) {

    data.resize(width * height * channels, 0);
}


int ImageG::sub2index(int x, int y) const {

    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::runtime_error("Requested position is out of bounds");
    }
    return (y*width + x)*channels;
}


int ImageG::sub2index(char ch, int x, int y) const {

    int chan;
    if (channels == 3) {
        if (ch == 'r') chan = 0;
        else if (ch == 'g') chan = 1;
        else if (ch == 'b') chan = 2;
        else {
            throw std::runtime_error("Invalid channel requested");
        }

        return sub2index(x, y) + chan;
    }
    if (channels == 1) {
        if (ch == 'y') chan = 0;
        else {
            throw std::runtime_error("Invalid channel requested");
        }
        return sub2index(x, y) + chan;
    }
    else {throw std::runtime_error("Unsupported number of channels for char access");}
}

int ImageG::sub2index(int ch, int x, int y) const {

    return sub2index(x, y) + ch;
}


unsigned char ImageG::operator()(char ch, int x, int y) const {

    return data[sub2index(ch, x, y)];
}

unsigned char ImageG::operator()(int ch, int x, int y) const {

    if (ch >= channels)
        throw std::runtime_error("Invalid channel requested");

    return data[sub2index(x, y) + ch];
}

unsigned char & ImageG::operator()(char ch, int x, int y) {

    return data[sub2index(ch, x, y)];
}

unsigned char & ImageG::operator()(int ch, int x, int y) {

    if (ch >= channels)
        throw std::runtime_error("Invalid channel requested");

    return data[sub2index(x, y) + ch];
}

ImageG ImageG::bn() const {

    if (channels == 1) {
        std::cout << "Image " << name << " is already in grayscale." << std::endl;
        return *this;
    }

    ImageG gray(1, width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int idx = sub2index(x, y);

            unsigned char r = data[idx];
            unsigned char g = data[idx + 1];
            unsigned char b = data[idx + 2];

            unsigned char grayVal = static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);

            int grayIdx = y * width + x;

            gray.data[grayIdx] = grayVal;
        }
    }

    return gray;
}

ImageG ImageG::crop(int cx, int cy, int w, int h) {

    if (cx < 0 || cy < 0 || cx + w > width || cy + h > height) {
        throw std::runtime_error("Crop coordinates are out of image bounds!");
    }

    if (w <= 0 || h <= 0) {
        throw std::runtime_error("Invalid crop dimensions (must be > 0)");
    }

    ImageG cropped(channels, w, h, name + "_crop");

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            int sourceX = cx + x;
            int sourceY = cy + y;

            int srcIdx = (sourceY * width + sourceX) * channels;
            int destIdx = (y * w + x) * channels;

            for (int k = 0; k < channels; k++) {
                cropped.data[destIdx + k] = data[srcIdx + k];
            }
        }
    }
    std::cout << "Image " << name << " cropped successfully." << std::endl;
    return cropped;
}

bool ImageG::equal(const ImageG &b) const {

    if (width != b.width || height != b.height || channels!=b.channels) return false;
    for (int n = 0; n < width * height * channels; n++) {
        if (data[n] != b.data[n]) {
            int c = n%channels;
            int x = (n/channels)%height;
            int y = (n/channels)/height;
            std::cout << n << ", " << c << " " << x << " " << y << std::endl;
            std::cout << static_cast<int>(data[n]) << std::endl;
            std::cout << static_cast<int>(b.data[n]) << std::endl;
            return false;
        }
    }
    return true;
}

float ImageG::avg_diff(const ImageG &b) const {

    float avg = 0;
    if (width != b.width || height != b.height || channels!=b.channels) return -1;
    for (int n = 0; n < width * height * channels; n++) {
        avg += std::abs(data[n] - b.data[n]);
    }
    avg /= (width * height * channels);
    return avg;
}

ImageG::values ImageG::conv_raw(const Kernel & k) const {

    std::vector<float> result(width*height*channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {

                float sum = 0.0f;
                for (int j = k.getMinIdx(); j <= k.getMaxIdx(); j++) {
                    for (int i = k.getMinIdx(); i <= k.getMaxIdx(); i++) {

                        int safeY = std::clamp(y + j, 0, height - 1);
                        int safeX = std::clamp(x + i, 0, width - 1);

                        sum += (*this)(c, safeX, safeY) * k.get(i, j);
                    }
                }
                int idx = sub2index(c, x, y);
                result[idx] = sum;
            }
        }
    }
    return result;
}

ImageG ImageG::conv(const Kernel &k) const {

    std::vector<float> raw_data = conv_raw(k);

    ImageG output(channels, width, height);
    for (size_t i = 0; i < raw_data.size(); i++) {

        output.data[i] = static_cast<unsigned char>(std::clamp(raw_data[i], 0.0f, 255.0f));
    }

    return output;
}

ImageG ImageG::operator+(const ImageG &b) const {

    if (width != b.width || height != b.height || (channels != b.channels and (b.channels != 1))) {
        throw std::invalid_argument("Cannot sum: dimensions mismatch or incompatible channels.");
    }

    ImageG output(channels, width, height, name + "_add");
    for (size_t i = 0; i < data.size(); i++) {
        size_t b_idx = (b.channels == 1) ? (i / channels) : i;
        int sub = (data[i] + b.data[b_idx]);
        output.data[i] = static_cast<unsigned char>(std::clamp(sub, 0, 255));
    }

    return output;
}

ImageG ImageG::operator-(const ImageG &b) const {

    if (width != b.width || height != b.height || (channels != b.channels and (b.channels != 1))) {
        throw std::invalid_argument("Cannot subtract: dimensions mismatch or incompatible channels.");
    }

    ImageG output(channels, width, height, name + "_sub");
    for (size_t i = 0; i < data.size(); i++) {
        size_t b_idx = (b.channels == 1) ? (i / channels) : i;
        int sub = (data[i] - b.data[b_idx]);
        output.data[i] = static_cast<unsigned char>(std::clamp(sub, 0, 255));
    }

    return output;
}

ImageG ImageG::operator*(const ImageG &b) const {

    if (width != b.width || height != b.height || (channels != b.channels and (b.channels != 1))) {
        throw std::invalid_argument("Cannot multiply: dimensions mismatch or incompatible channels.");
    }

    ImageG output(channels, width, height, name + "_mult");
    for (size_t i = 0; i < data.size(); i++) {
        size_t b_idx = (b.channels == 1) ? (i / channels) : i;
        int mul = (data[i] * b.data[b_idx]) / 255;
        output.data[i] = static_cast<unsigned char>(mul);
    }

    return output;
}

ImageG ImageG::abs_diff(const ImageG &b) const {

    if (width != b.width || height != b.height || channels != b.channels) {
        throw std::invalid_argument("Image dimensions and channels must match for this operation");
    }

    ImageG output(channels, width, height, name + "_absdiff");
    for (size_t i = 0; i < data.size(); i++) {
        int diff = std::abs(data[i] - b.data[i]);
        output.data[i] = static_cast<unsigned char>(diff);
    }
    return output;
}

ImageG ImageG::inv() const {

    ImageG output (channels, width, height, name + "_inv");

    for (size_t i = 0; i < data.size(); i++) {
        int val = 255 - data[i];
        output.data[i] = static_cast<unsigned char>(val);
    }
    return output;
}

ImageG ImageG::bin_blur(int d) const {

    Kernel kb(d);
    kb.set_bin();
    return this->conv(kb);
}

ImageG ImageG::f_bin_blur(int d) const {

    int ord = d-1;
    int it = ord/2;
    Kernel kb(3);
    kb.set_bin();
    ImageG output = this->conv(kb);
    for (int i = 1; i < it; i++) {
        output = output.conv(kb);
    }
    return output;
}

std::pair<ImageG, ImageG::values> ImageG::grad() const {

    ImageG gray = (channels > 1) ? this->bn() : *this;

    Kernel sx(3);
    sx.set_sobelX();
    Kernel sy(3);
    sy.set_sobelY();

    ImageG::values raw_sx = gray.conv_raw(sx);
    ImageG::values raw_sy = gray.conv_raw(sy);

    ImageG output(1, width, height, name + "_sobel");
    ImageG::values direction(width*height);

    for (int i = 0; i < width * height; i++) {

        float magnitude = std::sqrt(raw_sx[i] * raw_sx[i] + raw_sy[i] * raw_sy[i]);

        output.data[i] = static_cast<unsigned char>(std::clamp(magnitude, 0.0f, 255.0f));
        direction[i] = std::atan2(raw_sy[i], raw_sx[i]);
    }

    return {output, direction};
}

ImageG::values ImageG::laplacian() const {

    ImageG gray = (channels > 1) ? this->bn() : *this;
    Kernel lap(3);
    lap.set_laplacian();
    return gray.conv_raw(lap);
}

ImageG::values ImageG::LoG(int d) const {

    ImageG gray = (channels > 1) ? this->bn() : *this;
    ImageG blurred = gray.f_bin_blur(d);
    return blurred.laplacian();
}

ImageG ImageG::zc_LoG(int d, float threshold) const {
    std::vector<float> Log = this->LoG(d);

    ImageG output(1, width, height, name + "_zerocross");
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {

            int idx = y * width + x;
            float val = Log[idx];

            int right = idx + 1;
            int down  = idx + width;

            bool is_edge = false;

            if (val * Log[right] <= 0.0f) {
                if (std::abs(val - Log[right]) > threshold) {
                    is_edge = true;
                }
            }

            if (!is_edge && (val * Log[down] <= 0.0f)) {
                if (std::abs(val - Log[down]) > threshold) {
                    is_edge = true;
                }
            }

            output.data[idx] = is_edge ? 255 : 0;
        }
    }
    return output;
}

void ImageG::save(const std::string &filename) {

    size_t pos = filename.find_last_of(".");
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid filename: missing extension!");
    }

    std::string ext = filename.substr(pos + 1);
    for (auto &c : ext) c = tolower(c);

    int success = 0;

    if (ext == "png") {
        success = stbi_write_png(filename.c_str(), width, height, channels, data.data(), width * channels);
    }
    else if (ext == "jpg" || ext == "jpeg") {
        success = stbi_write_jpg(filename.c_str(), width, height, channels, data.data(), 70);
    }
    else {
        std::cout << "Unknown extension '" << ext << "'. Saving as PNG instead." << std::endl;
        std::string newName = filename + ".png";
        success = stbi_write_png(newName.c_str(), width, height, channels, data.data(), width * channels);
    }

    if (!success) {
        throw std::runtime_error("Error saving file: " + filename);
    }

    std::cout << "Image saved successfully: " << filename << std::endl;
}
