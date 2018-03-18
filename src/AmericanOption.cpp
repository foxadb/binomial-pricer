#include "AmericanOption.hpp"

#include <iostream>
#include <string>
#include <cmath>

AmericanOption::AmericanOption(double X0, double K, double r, double sigma, double T, int N) {
    this->X0 = X0;
    this->K = K;
    this->r = r;
    this->sigma = sigma;
    this->T = T;
    this->N = N;

    this->h = T / N;
    this->u = std::exp(sigma * std::sqrt(this->h));
    this->d = std::exp(-sigma * std::sqrt(this->h));
    this->p = (d - std::exp(r * this->h)) / (d - u);

    this->price = new double*[N];
    for (int i = 0; i < N; ++i) {
        this->price[i] = new double[N];
    }
}

AmericanOption::~AmericanOption() {
    for (int i = 0; i < N; ++i) {
        delete [] this->price[i];
    }
    delete [] this->price;
}

std::string AmericanOption::toString() {
    std::string result = "AmericanOption { X0 = " + std::to_string(this->X0)
            + ", K = " + std::to_string(this->K)
            + ", r = " + std::to_string(this->r)
            + ", sigma = " + std::to_string(this->sigma)
            + ", T = " + std::to_string(this->T)
            + ", N = " + std::to_string(this->N)
            +" }";
    return result;
}

double AmericanOption::payoff(double stock) {
    return std::fmax(this->K - stock, 0);
}

void AmericanOption::pricing() {
    // Initialization
    for (int j = 0; j < N; ++j) {
        double mult = std::pow(this->u, j) * std::pow(this->d, N - 1 - j);
        this->price[N - 1][j] = this->payoff(this->X0 * mult);
    }

    for (int i = N - 2; i >= 0; --i) {
        double mult = std::pow(this->d, i);
        for (int j = 0; j <= i; ++j) {
            // Compute payoff
            double payoff = this->payoff(this->X0 * mult);

            // Compute price
            double price = std::fmax(std::exp(-this->r * this->h)
                                     * (this->price[i+1][j] * (1 - this->p)
                                     + this->price[i+1][j+1] * this->p),
                    payoff);

            // Storage price in the binomial tree
            this->price[i][j] = price;

            mult *= this->u / this->d;
        }
    }
}

double AmericanOption::getPrice(int i, int j) {
    return this->price[i][j];
}
