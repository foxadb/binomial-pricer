/* kernel.cl
 * American Option Binomial Pricing
 * Device code
 */

__kernel void initial_prices(
        __global double* prices,
        const int N,
        const double X0,
        const double K,
        const double u,
        const double d
        ) {
    int j = get_global_id(0);
    double mult = pow(u, j) * pow(d, N - 1 - j);
    prices[(N - 1) * N + j] = fmax(K - X0 * mult, 0);
}

__kernel void binomial_pricer(
        __global double* prices,
        const int line,
        const int N,
        const double X0,
        const double K,
        const double r,
        const double h,
        const double u,
        const double d,
        const double p
        ) {
    int column = get_global_id(0);

    // Compute multiplier
    double mult = pow(d, line - column) * pow(u, column);

    // Compute payoff
    double payoff = fmax(K - X0 * mult, 0);

    // Compute price
    double price = fmax(exp(-r * h)
            * (prices[(line + 1) * N + column] * (1 - p)
                + prices[(line + 1) * N + column + 1] * p),
            payoff);

    // Storage price in the binomial tree
    prices[line * N + column] = price;
}
