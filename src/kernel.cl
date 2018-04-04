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
    int i = get_global_id(0);
    double mult = pow(u, i) * pow(d, N - 1 - i);
    prices[i] = fmax(K - X0 * mult, 0);
}

__kernel void binomial_pricer(
        const int N,
        const double X0,
        const double K,
        const double r,
        const double h,
        const double u,
        const double d,
        const double p,
        const int groupSize,
        __global double* pricesIn,
        __global double* pricesOut,
        const int nodesNb
        ) {
    int startIndex = get_global_id(0) * groupSize;
    int endIndex = min(startIndex + groupSize, nodesNb);

    for (int i = startIndex; i < endIndex; ++i) {
        // Compute multiplier
        double mult = pow(d, nodesNb - 1 - i) * pow(u, i);

        // Compute payoff
        double payoff = fmax(K - X0 * mult, 0);

        // Compute price
        double price = fmax(exp(-r * h)
                * (pricesIn[i] * (1 - p) + pricesIn[i + 1] * p),
                payoff);

        // Storage price in output buffer
        pricesOut[i] = price;
    }
}
