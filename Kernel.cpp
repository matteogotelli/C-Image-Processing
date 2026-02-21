//
// Created by Matteo on 11/02/2026.
//

#include "Kernel.hpp"

Kernel::Kernel(int d) {

    if (d % 2 == 0) {
        throw std::runtime_error("Kernel size must be an odd number");
    }
    dim = d;
    min_idx = - dim / 2;
    max_idx = dim / 2;
    values.resize(dim*dim, 0.0f);
}

int Kernel::sub2ind(int i, int j) const {

    int row = i + max_idx;
    int col = j + max_idx;
    return col * dim + row;
}

void Kernel::set(int i, int j, float val) {

    values[sub2ind(i, j)] = val;
}

float Kernel::get(int i, int j) const {

    return values[sub2ind(i, j)];
}

void Kernel::normalize() {
    float sum = 0.0f;
    for (int j = min_idx; j <= max_idx; j++) {
        for (int i = min_idx; i <= max_idx; i++) {
            sum += get(i,j);
        }
    }

    if (sum==0.0f) return;

    for (int j = min_idx; j <= max_idx; j++) {
        for (int i = min_idx; i <= max_idx; i++) {
            float val = get(i,j);
            set(i,j, val/sum);
        }
    }
}

void Kernel::print() const {
    for (int j = min_idx; j <= max_idx; j++) {
        for (int i = min_idx; i <= max_idx; i++) {
            std::cout << values[sub2ind(i, j)] << " ";
        }
        std::cout << std::endl;
    }
}

void Kernel::set_bin() {

    std::vector<float> pascal(dim);
    int n = dim - 1;

    pascal[0] = 1.0f;
    for (int i = 0; i < n; i++) {
        pascal[i + 1] = pascal[i] * (n-i) / (i+1);
    }

    for (int j = min_idx; j <= max_idx; j++) {
        for (int i = min_idx; i <= max_idx; i++) {
            float val = pascal[i + max_idx] * pascal[j + max_idx];
            set(i, j, val);
        }
    }
    normalize();
}

void Kernel::set_sobelX() {

    if (dim!=3) throw std::runtime_error("Kernel size must be 3");

    set(-1,-1,-1); set(0, -1, 0); set(1, -1, 1);
    set(-1, 0, -2); set(0, 0, 0); set(1, 0, 2);
    set(-1, 1, -1); set(0, 1, 0); set(1, 1, 1);

}

void Kernel::set_sobelY() {

    if (dim!=3) throw std::runtime_error("Kernel size must be 3");

    set(-1,-1,-1); set(0, -1, -2); set(1, -1, -1);
    set(-1, 0, 0); set(0, 0, 0); set(1, 0, 0);
    set(-1, 1, 1); set(0, 1, 2); set(1, 1, 1);

}

void Kernel::set_laplacian() {

    if (dim!=3) throw std::runtime_error("Kernel size must be 3");

    set(-1,-1,-1); set(0, -1, -1); set(1, -1, -1);
    set(-1, 0, -1); set(0, 0, 8); set(1, 0, -1);
    set(-1, 1, -1); set(0, 1, -1); set(1, 1, -1);
}
