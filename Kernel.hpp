//
// Created by Matteo on 11/02/2026.
//

#ifndef IMAGE_PROCESSING_KERNEL_HPP
#define IMAGE_PROCESSING_KERNEL_HPP

#include <vector>
#include <numeric> // per std::accumulate
#include <cmath>
#include <algorithm>
#include <iostream>

class Kernel {
    int dim;
    int min_idx, max_idx;
    std::vector<float> values;

public:
    Kernel() = default;
    explicit Kernel(int d);

    int getMinIdx() const {return min_idx;}
    int getMaxIdx() const {return max_idx;}

    int sub2ind(int i, int j) const;
    void set(int i, int j, float val);
    float get(int i, int j) const;
    void normalize();
    void print() const;

    void set_bin();
    void set_sobelX();
    void set_sobelY();
    void set_laplacian();

};

#endif //IMAGE_PROCESSING_KERNEL_HPP