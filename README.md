# ImageG: C++ Image Processing Engine

A lightweight C++ image pre-processing engine built from scratch. Developed to apply concepts from the **Algorithms (C++)** and **Medical Image Processing** courses at Politecnico di Milano.

The goal was to apply theoretical knowledge of Image Processing to a practical knowledge of C++ by building a dependency-free engine. Instead of relying on black-box frameworks like OpenCV, `ImageG` directly implements the core mathematical concepts of Computer Vision to deeply understand how they work under the hood. 

It handles memory safely via `std::vector` and focuses on spatial convolutions, kernel logic, and edge detection algorithms (such as Zero-Crossing), which are foundational in medical image segmentation.

## Features

- **No external heavy dependencies** (only `stb_image` for image I/O).
- **Core Math Engine:** Custom kernel matrix generation and raw/byte spatial convolutions.
- **Edge Detection:**
  - Sobel Operator (`grad`)
  - Laplacian of Gaussian (`LoG`)
  - Zero-Crossing (`zc_LoG`) for 1-pixel thin, mathematically accurate continuous edges.
- **Filtering & Manipulation:**
  - Iterative Binomial Blur (`f_bin_blur`)
  - Grayscale Conversion
  - Image Cropping
- **Arithmetic Overloading:** Easily combine images using standard C++ operators (`+`, `-`, `*`) for image masking and blending.

## Usage Example

The library is designed to be highly readable. Here is a quick example of how to extract 1-px thin edges using the Zero-Crossing algorithm:

```cpp
#include "ImageG.hpp"

int main() {
    // Load image
    ImageG img("test.jpg");
    
    // Convert to grayscale
    ImageG gray = img.bn();
    
    // Apply Zero Crossing Edge Detection (using discretized Gaussian blurring masks and Laplacian mask) 
    // Parameters: blur diameter = 5, zero-crossing threshold = 15.0f
    ImageG thin_edges = gray.zc_LoG(5, 15.0f);
    
    // Save result
    thin_edges.save("output_edges.jpg");

    return 0;
}

## How to Build

Simply compile the source files together using your preferred C++ compiler (C++17 recommended).

```bash
g++ main.cpp ImageG.cpp Kernel.cpp -o image\_processor
./image\_processor
