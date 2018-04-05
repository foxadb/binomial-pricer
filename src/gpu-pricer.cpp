#include "AmericanOptionGPU.hpp"

#include <iostream>
#include <ctime>

int main(int argc, char *argv[]) {
    if (argc == 8) {
        // Parse arguments
        double X0 = std::atof(argv[1]);
        double K = std::atof(argv[2]);
        double r = std::atof(argv[3]);
        double sigma = std::atof(argv[4]);
        double T = std::atof(argv[5]);
        int N = std::atoi(argv[6]);
        int stepSize = std::atoi(argv[7]);

        // Create american option
        AmericanOptionGPU *option = new AmericanOptionGPU(X0, K, r, sigma, T, N);

        // Output option details
        std::cout << option->toString() << std::endl;

        // Pricing with timer
        std::clock_t start = std::clock();
        double price = option->linearPricing(stepSize);
        std::clock_t end = std::clock();
        double duration = (double)(end - start) / CLOCKS_PER_SEC;

        // Print price at 0
        std::cout << "Price: " << price << std::endl;

        // Show timer
        std::cout << "Computation time: " << duration << " s" << std::endl;

        // Free memory
        delete option;
    } else {
        std::cout << "Usage: pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ] [ work group size]" << std::endl;
    }

    // Program end
    return 0;
}
