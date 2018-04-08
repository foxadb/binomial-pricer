#include "AmericanOptionCPU.hpp"

#include <iostream>
#include <string>
#include <cmath>

AmericanOptionCPU::AmericanOptionCPU(float X0, float K, float r, float sigma, float T, int N) {
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

    this->prices = new float*[N];
    for (int i = 0; i < N; ++i) {
        this->prices[i] = new float[N];
    }
}

AmericanOptionCPU::~AmericanOptionCPU() {
    for (int i = 0; i < this->N; ++i) {
        delete this->prices[i];
    }
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

float AmericanOptionCPU::payoff(float stock) {
    return std::fmax(this->K - stock, 0);
}

float AmericanOptionCPU::pricing() {
    // Initialization
    for (int j = 0; j < N; ++j) {
        float mult = std::pow(this->u, j) * std::pow(this->d, N - 1 - j);
        this->prices[N - 1][j] = this->payoff(this->X0 * mult);
    }

    // Compute the discount factor
    float discountFactor = 1 / std::exp(this->r * this->h);

    for (int i = N - 2; i >= 0; --i) {
        float mult = std::pow(this->d, i);
        for (int j = 0; j <= i; ++j) {
            // Compute payoff
            float payoff = this->payoff(this->X0 * mult);

            // Compute price
            float price = std::fmax(discountFactor
                                     * (this->prices[i + 1][j] * (1 - this->p)
                                     + this->prices[i + 1][j + 1] * this->p),
                    payoff);

            // Storage price in the binomial tree
            this->prices[i][j] = price;

            // Update mult factor
            mult *= this->u / this->d;
        }
    }

    return this->prices[0][0];
}

float AmericanOptionCPU::getPrice(int i, int j) {
    return this->prices[i][j];
}
