#include "AmericanOptionCPU.hpp"

#include <iostream>
#include <string>
#include <cmath>

AmericanOptionCPU::AmericanOptionCPU(double X0, double K, double r, double sigma, double T, int N) {
    this->X0 = X0;
    this->K = K;
    this->r = r;
    this->sigma = sigma;
    this->T = T;
    this->N = N;

    this->h = T / N;
    this->u = std::exp(sigma * std::sqrt(this->h));
    this->d = 1 / this->u;
    this->p = (d - std::exp(r * this->h)) / (d - u);

    this->prices = new double[N * N];
}

AmericanOptionCPU::~AmericanOptionCPU() {
    delete [] this->prices;
}

std::string AmericanOptionCPU::toString() {
    std::string result = "AmericanOptionCPU { X0 = " + std::to_string(this->X0)
            + ", K = " + std::to_string(this->K)
            + ", r = " + std::to_string(this->r)
            + ", sigma = " + std::to_string(this->sigma)
            + ", T = " + std::to_string(this->T)
            + ", N = " + std::to_string(this->N)
            +" }";
    return result;
}

double AmericanOptionCPU::payoff(double stock) {
    return std::fmax(this->K - stock, 0);
}

void AmericanOptionCPU::pricing() {
    // Initialization
    for (int j = 0; j < N; ++j) {
        double mult = std::pow(this->u, j) * std::pow(this->d, N - 1 - j);
        this->prices[(N - 1) * N + j] = this->payoff(this->X0 * mult);
    }

    for (int i = N - 2; i >= 0; --i) {
        double mult = std::pow(this->d, i);
        for (int j = 0; j <= i; ++j) {
            // Compute payoff
            double payoff = this->payoff(this->X0 * mult);

            // Compute price
            double price = std::fmax(std::exp(-this->r * this->h)
                                     * (this->prices[(i + 1) * N + j] * (1 - this->p)
                                     + this->prices[(i + 1) * N + j + 1] * this->p),
                    payoff);

            // Storage price in the binomial tree
            this->prices[i * N + j] = price;

            // Update mult factor
            mult *= this->u / this->d;
        }
    }
}

double AmericanOptionCPU::getPrice(int i, int j) {
    return this->prices[i * N + j];
}
