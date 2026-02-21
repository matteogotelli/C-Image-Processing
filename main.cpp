#include <iostream>
#include <chrono>
#include "ImageG.hpp"

int main() {
    std::cout << "=========================================\n";
    std::cout << "       ImageG - Processing Showcase      \n";
    std::cout << "=========================================\n\n";

    try {

        std::string input_file = "test.jpg";
        std::cout << "Loading test image: '" << input_file << "'\n\n";
        ImageG img(input_file);

        std::cout << std::endl;
        std::cout << "[1/5] Converting to Grayscale..." << std::endl;
        ImageG gray = img.bn();
        gray.save("output_1_gray.jpg");
        std::cout << std::endl;

        std::cout << "[2/5] Applying Iterative Binomial Blur..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        ImageG blur = img.f_bin_blur(15);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        blur.save("output_2_blur.jpg");
        std::cout << "      -> Completed in " << duration.count() << " ms." << std::endl;
        std::cout << std::endl;

        std::cout << "[3/5] Extracting Edges (Sobel Operator)..." << std::endl;
        auto gradient = gray.grad();
        ImageG edges = gradient.first;
        edges.save("output_3_sobel.jpg");
        std::cout << std::endl;

        std::cout << "[4/5] Extracting Thin Edges (Zero Crossing LoG)..." << std::endl;
        ImageG thin_edges = gray.zc_LoG(9, 20.0f);
        thin_edges.save("output_4_zerocrossing.jpg");
        std::cout << std::endl;

        std::cout << "[5/5] Testing Arithmetic Operations (Masking)..." << std::endl;
        ImageG masked = img * edges;
        masked.save("output_5_masked.jpg");

        std::cout << "\n=========================================\n";
        std::cout << " Processing completed successfully!      \n";
        std::cout << " Please check the 'output_*.jpg' files.  \n";
        std::cout << "=========================================\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR]: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}