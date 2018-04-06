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
        int groupSize = std::atoi(argv[7]);

        // Create american option
        AmericanOptionGPU *option = new AmericanOptionGPU(X0, K, r, sigma, T, N);

        // Output option details
        std::cout << option->toString() << std::endl;

        //// Ping Pong method

        // Pricing
        std::cout << "=== Ping Pong method ===" << std::endl;
        std::clock_t ppStart = std::clock();
        double ppPrice = option->pingPongPricing(groupSize);
        std::clock_t ppEnd = std::clock();
        double ppDuration = (double)(ppEnd - ppStart) / CLOCKS_PER_SEC;

        // Print price at 0
        std::cout << "Price: " << ppPrice << std::endl;

        // Show timer
        std::cout << "Timer: " << ppDuration << " s" << std::endl;

        //// Branch lift method

        // Pricing
        std::cout << "=== Branch Climb method ===" << std::endl;
        std::clock_t bcStart = std::clock();
        double bcPrice = option->branchClimbPricing();
        std::clock_t bcEnd = std::clock();
        double bcDuration = (double)(bcEnd - bcStart) / CLOCKS_PER_SEC;

        // Print price at 0
        std::cout << "Price: " << bcPrice << std::endl;

        // Show timer
        std::cout << "Timer: " << bcDuration << " s" << std::endl;

        // Free memory
        delete option;
    } else {
        std::cout << "Usage: pricer [ X0 ] [ K ] [ r ] [ sigma ] [ T ] [ N ] [ work group size]" << std::endl;
    }

    // Program end
    return 0;
}
